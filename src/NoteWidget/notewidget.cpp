#include "notewidget.h"
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

using namespace Utils;

QList<NoteWidget*> NoteWidget::existingNotes;

NoteWidget::NoteWidget(QWidget *parent, const QString &filePath, bool restored)
    : QWidget(parent),
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

    QJsonObject noteObject;
    noteObject["title"] = noteTitle;
    noteObject["content"] = noteContent;
    noteObject["posX"] = pos().x();
    noteObject["posY"] = pos().y();
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

            QPoint position(noteObject.value("posX").toInt(), noteObject.value("posY").toInt());
            restorePosition(position);

            isPinned = noteObject.value("pinned").toBool();
            ui->pinButton->setChecked(isPinned);
            ui->pinButton->setIcon(getIcon(2, isPinned));
            togglePinnedState();
        }
    }

    fadeIn();
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
    new NoteWidget();
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
    QPalette palette = ui->noteTitleLineEdit->palette();
    if (getTheme() == "dark") {
        palette.setColor(QPalette::Text, getAccentColor("dark2"));
    } else {
        palette.setColor(QPalette::Text, getAccentColor("light3"));
    }
    ui->noteTitleLineEdit->setPalette(palette);

    ui->noteTitleLineEdit->setPlaceholderText("Name this note");
}

void NoteWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPosition = event->pos();
        this->setMouseTracking(true);
    }
}

void NoteWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint newPos = this->pos() + (event->pos() - dragStartPosition);
        this->move(newPos);
    }
}

void NoteWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        this->releaseMouse();
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

