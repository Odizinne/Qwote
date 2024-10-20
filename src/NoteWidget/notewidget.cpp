#include "notewidget.h"
#include "qwote.h"
#include "ui_notewidget.h"
#include "utils.h"
#include <QMouseEvent>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QTextStream>
#include <QPainter>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QFont>

using namespace Utils;
const int resizeMargin = 7;
const QString NoteWidget::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/Qwote/settings.json";

QList<NoteWidget*> NoteWidget::existingNotes;

NoteWidget::NoteWidget(QWidget *parent, const QString &filePath, bool restored, Qwote *qwoteInstance)
    : QWidget(parent),
    qwoteInstance(qwoteInstance),
    ui(new Ui::NoteWidget),
    isDragging(false),
    filePath(filePath),
    isRestored(restored),
    isPinned(false)
{
    ui->setupUi(this);
    setWindowTitle("New note");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);

    setTitleColor();
    loadSettings();

    ui->newButton->setIcon(getIcon(1, false));
    ui->pinButton->setIcon(getIcon(2, false));
    ui->closeButton->setIcon(getIcon(3, false));

    if (isRestored) {
        loadNoteFromFile();
        existingNotes.append(this);
    } else {
        createNewNoteFile();
        placeNote();
        existingNotes.append(this);
        saveNote();
    }

    connect(ui->closeButton, &QToolButton::clicked, this, &NoteWidget::deleteNote);
    connect(ui->pinButton, &QToolButton::clicked, this, &NoteWidget::togglePinnedState);
    connect(ui->newButton, &QToolButton::clicked, this, &NoteWidget::createNewNote);
    connect(ui->noteTextEdit, &QTextEdit::textChanged, this, &NoteWidget::saveNote);
    connect(ui->noteTitleLineEdit, &QLineEdit::textChanged, this, &NoteWidget::onNoteTitleChanged);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(250);
    animation->setStartValue(0);
    animation->setEndValue(1);
    setWindowOpacity(0);
    show();
    animation->start();
}

NoteWidget::~NoteWidget() {
    delete ui;
}

void NoteWidget::togglePinnedState() {
    isPinned = ui->pinButton->isChecked();
    setWindowFlag(Qt::WindowStaysOnTopHint, isPinned);
    ui->pinButton->setIcon(getIcon(2, isPinned));
    show();
    saveNote();
}

void NoteWidget::placeNote() {
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

    // Calculate the desired position (20px from the top and right)
    int posX = screenGeometry.right() - this->width() - 20; // Right border minus width and margin
    int posY = 20; // 20 pixels from the top

    // Check for overlap with existing notes
    QRect newNoteRect(posX, posY, this->width(), this->height());
    const int offset = 20; // Margin for avoiding overlap

    // Adjust position if overlapping
    for (NoteWidget *existingNote : existingNotes) {
        QRect existingNoteRect = existingNote->geometry();
        if (newNoteRect.intersects(existingNoteRect)) {
            // If overlap is detected, adjust the position
            posY += existingNoteRect.height() + offset; // Move down by the height of the existing note + offset
            newNoteRect.moveTo(posX, posY); // Update the new rectangle with the new position
            // Reset posY to top if it exceeds the screen height
            if (posY + this->height() > screenGeometry.bottom()) {
                posY = 20; // Reset to top
            }
        }
    }

    // Move the new note to the calculated position
    this->move(newNoteRect.topLeft());
}
void NoteWidget::createNewNoteFile() {
    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(appDataLocation);
    if (!dir.exists()) {
        dir.mkpath(appDataLocation);
    }

    int i = 1;
    QString baseFileName;
    do {
        baseFileName = QString("note-%1.json").arg(i++);
    } while (QFile::exists(appDataLocation + "/" + baseFileName));

    filePath = appDataLocation + "/" + baseFileName;

    //saveNote();
}

void NoteWidget::saveNote() {
    if (filePath.isEmpty()) {
        return;
    }

    QString noteTitle = ui->noteTitleLineEdit->text();
    QString noteContent = ui->noteTextEdit->toPlainText();
    int fontSize = ui->noteTextEdit->font().pointSize();  // Get current font size

    QJsonObject noteObject;
    noteObject["title"] = noteTitle;
    noteObject["content"] = noteContent;
    noteObject["fontSize"] = fontSize;  // Save font size
    noteObject["posX"] = pos().x();
    noteObject["posY"] = pos().y();
    noteObject["width"] = this->width();
    noteObject["height"] = this->height();
    noteObject["pinned"] = isPinned;

    QJsonDocument doc(noteObject);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void NoteWidget::loadNoteFromFile() {
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
        if (jsonDoc.isObject()) {
            QJsonObject noteObject = jsonDoc.object();
            QString noteTitle = noteObject.value("title").toString();
            QString noteContent = noteObject.value("content").toString();
            setWindowTitle(noteTitle);
            setNoteTitle(noteTitle);
            setNoteContent(noteContent);

            int fontSize = noteObject.value("fontSize").toInt(11);  // Default to 11 if not found
            setTextEditFontSize(fontSize);

            QPoint position(noteObject.value("posX").toInt(), noteObject.value("posY").toInt());
            restorePosition(position);

            int width = noteObject.value("width").toInt(this->width());
            int height = noteObject.value("height").toInt(this->height());
            this->resize(width, height);

            isPinned = noteObject.value("pinned").toBool();
            ui->pinButton->setChecked(isPinned);
            ui->pinButton->setIcon(getIcon(2, isPinned));
            togglePinnedState();
        }
    }

    fadeIn();
}

void NoteWidget::setTextEditFontSize(int fontSize) {
    QFont font = ui->noteTextEdit->font();
    font.setPointSize(fontSize);
    ui->noteTextEdit->setFont(font);
}

void NoteWidget::setNoteTitle(const QString &title) {
    ui->noteTitleLineEdit->setText(title);
}

void NoteWidget::setNoteContent(const QString &content) {
    ui->noteTextEdit->setPlainText(content);
}

void NoteWidget::deleteNote() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(250);
    animation->setStartValue(1);
    animation->setEndValue(0);

    connect(animation, &QPropertyAnimation::finished, this, [this, animation]() {
        // Cleanup the animation
        animation->deleteLater(); // Prevent memory leak
        if (!filePath.isEmpty()) {
            QFile::remove(filePath);
        }
        existingNotes.removeAll(this);
        this->deleteLater(); // Safely delete the widget
    });

    animation->start(); // Start the fade-out animation
}


void NoteWidget::createNewNote() {
    //NoteWidget *newNote = new NoteWidget();
    //new NoteWidget();
    if (qwoteInstance) {
        qwoteInstance->createNewNote(); // Call Qwote's createNewNote
    }
}

void NoteWidget::savePosition() {
    if (!filePath.isEmpty()) {
        saveNote();
    }
}

void NoteWidget::restorePosition(const QPoint &position) {
    move(position);
}

void NoteWidget::onNoteTitleChanged() {
    saveNote();
    setWindowTitle(ui->noteTitleLineEdit->text());
}

void NoteWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    QColor backgroundColor;
    backgroundColor = this->palette().color(QPalette::Window);

    painter.setBrush(backgroundColor);
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(rect(), 8, 8);
}

void NoteWidget::setTitleColor() {
    QStringList placeholders = {
        "shopping list",
        "Give it a name",
        "Name this note",
        "My super note",
        "Remember This",
        "Quick Thoughts",
        "Note to Self",
        "Code Snippets",
        "Work in Progress",
        "Ideas",
        "Workflows"
    };

    int randomIndex = QRandomGenerator::global()->bounded(placeholders.size());
    QString randomPlaceholder = placeholders[randomIndex];

    QPalette palette = ui->noteTitleLineEdit->palette();
    if (getTheme() == "dark") {
        palette.setColor(QPalette::Text, getAccentColor("dark2"));
    } else {
        palette.setColor(QPalette::Text, getAccentColor("light3"));
    }
    ui->noteTitleLineEdit->setPalette(palette);

    ui->noteTitleLineEdit->setPlaceholderText(randomPlaceholder);
}

void NoteWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (resizeDirection != Qt::Edges()) {
            isResizing = true;
            this->grabMouse(); // Capture the mouse for resizing
        } else {
            isDragging = true;
            dragStartPosition = event->pos();
            this->setMouseTracking(true);
        }
    }
}

void NoteWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        // Handle window dragging
        QPoint newPos = this->pos() + (event->pos() - dragStartPosition);
        this->move(newPos);
    } else if (isResizing) {
        // Handle window resizing
        QRect currentGeometry = this->geometry();
        QPoint globalMousePos = event->globalPos();

        if (resizeDirection & Qt::LeftEdge) {
            int newX = globalMousePos.x();
            if (currentGeometry.right() - newX >= minimumWidth()) {
                currentGeometry.setLeft(newX);
            }
        }
        if (resizeDirection & Qt::RightEdge) {
            int newWidth = globalMousePos.x() - currentGeometry.left();
            if (newWidth >= minimumWidth()) {
                currentGeometry.setWidth(newWidth);
            }
        }
        if (resizeDirection & Qt::TopEdge) {
            int newY = globalMousePos.y();
            if (currentGeometry.bottom() - newY >= minimumHeight()) {
                currentGeometry.setTop(newY);
            }
        }
        if (resizeDirection & Qt::BottomEdge) {
            int newHeight = globalMousePos.y() - currentGeometry.top();
            if (newHeight >= minimumHeight()) {
                currentGeometry.setHeight(newHeight);
            }
        }

        this->setGeometry(currentGeometry);
    } else {
        // Check if the cursor is near the edges for resizing
        updateCursorShape(event->pos());
    }
}

void NoteWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizing = false;
        this->releaseMouse(); // Release mouse capture when resizing is done
        savePosition();
    }
}

void NoteWidget::fadeIn() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(250);
    animation->setStartValue(0);
    animation->setEndValue(1);
    setWindowOpacity(0);
    show();
    animation->start();
}

void NoteWidget::updateCursorShape(const QPoint &pos) {
    Qt::Edges detectedEdges;

    if (pos.x() <= resizeMargin) {
        detectedEdges |= Qt::LeftEdge;
    } else if (pos.x() >= this->width() - resizeMargin) {
        detectedEdges |= Qt::RightEdge;
    }

    if (pos.y() <= resizeMargin) {
        detectedEdges |= Qt::TopEdge;
    } else if (pos.y() >= this->height() - resizeMargin) {
        detectedEdges |= Qt::BottomEdge;
    }

    // Set appropriate cursor based on detected edges
    if (detectedEdges == (Qt::LeftEdge | Qt::TopEdge) || detectedEdges == (Qt::RightEdge | Qt::BottomEdge)) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (detectedEdges == (Qt::RightEdge | Qt::TopEdge) || detectedEdges == (Qt::LeftEdge | Qt::BottomEdge)) {
        setCursor(Qt::SizeBDiagCursor);
    } else if (detectedEdges & (Qt::LeftEdge | Qt::RightEdge)) {
        setCursor(Qt::SizeHorCursor);
    } else if (detectedEdges & (Qt::TopEdge | Qt::BottomEdge)) {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    resizeDirection = detectedEdges; // Store the current edge for resizing
}

void NoteWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        QString text = event->text();  // Get the actual text representation

        if (text == "+" || text == "=") {
            increaseFontSize();  // Ctrl + or Ctrl =
        } else if (text == "-") {
            decreaseFontSize();  // Ctrl -
        } else if (text == "0") {
            resetFontSize();  // Ctrl 0
        }
    }

    QWidget::keyPressEvent(event);
}

void NoteWidget::increaseFontSize() {
    QFont font = ui->noteTextEdit->font();
    int currentSize = font.pointSize();
    if (currentSize < 16) {
        font.setPointSize(currentSize + 1);
        ui->noteTextEdit->setFont(font);
        saveNote();
    }
}

void NoteWidget::decreaseFontSize() {
    QFont font = ui->noteTextEdit->font();
    int currentSize = font.pointSize();
    if (currentSize > 8) {
        font.setPointSize(currentSize - 1);
        ui->noteTextEdit->setFont(font);
        saveNote();
    }
}

void NoteWidget::resetFontSize() {
    setTextEditFontSize(11);
    saveNote();
}

void NoteWidget::loadSettings()
{
    QFont currentFont = ui->noteTextEdit->font();
    int currentSize = currentFont.pointSize();
    int titleSize = 11;

    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        QFont defaultFont;
        defaultFont.setFamily("Consolas");
        defaultFont.setPointSize(11);
        ui->noteTextEdit->setFont(defaultFont);
        defaultFont.setBold(true);
        ui->noteTitleLineEdit->setFont(defaultFont);

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
                QFont userFont;
                userFont.setFamily(settings.value("font").toString());
                userFont.setPointSize(currentSize);
                ui->noteTextEdit->setFont(userFont);
                userFont.setPointSize(titleSize);
                userFont.setBold(true);
                ui->noteTitleLineEdit->setFont(userFont);
            }
            file.close();
        }
    }
}
