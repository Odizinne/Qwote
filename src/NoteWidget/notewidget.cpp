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
#include <QTextBlock>
#include <QSettings>

using namespace Utils;
const int resizeMargin = 7;

QList<NoteWidget*> NoteWidget::existingNotes;

NoteWidget::NoteWidget(QWidget *parent, const QString &filePath, bool restored, Qwote *qwoteInstance)
    : QWidget(parent)
    , qwoteInstance(qwoteInstance)
    , ui(new Ui::NoteWidget)
    , filePath(filePath)
    , isDragging(false)
    , isRestored(restored)
    , isPinned(false)
    , isResizing(false)
    , ctrlPressed(false)
    , resizeDirection(Qt::Edges())
    , opacity(255)
    , roundedCorners(true)
{
    ui->setupUi(this);
    setWindowTitle(tr("New note"));
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);

    setTitle();
    setTitleTransparency();
    setContentTransparency();
    setButtons();
    setTextEditButtons();
    loadSettings();
    ui->frame->setVisible(false);

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
    connect(ui->plusButton, &QToolButton::pressed, this, &NoteWidget::increaseFontSize);
    connect(ui->minusButton, &QToolButton::pressed, this, &NoteWidget::decreaseFontSize);
    connect(ui->boldButton, &QToolButton::toggled, this, &NoteWidget::onBoldButtonStateChanged);
    connect(ui->italicButton, &QToolButton::toggled, this, &NoteWidget::onItalicButtonStateChanged);
    connect(ui->underlineButton, &QToolButton::toggled, this, &NoteWidget::onUnderlineButtonStateChanged);
    connect(ui->editorToolsButton, &QToolButton::toggled, this, &NoteWidget::onEditorToolsButtonStateChanged);
    connect(ui->strikethroughButton, &QToolButton::toggled, this, &NoteWidget::onStrikethroughButtonStateChanged);
    connect(ui->bulletlistButton, &QToolButton::toggled, this, &NoteWidget::onBulletListButtonStateChanged);


    fadeIn();
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
    int posX = screenGeometry.right() - this->width() - 20;
    int posY = 20;

    QRect newNoteRect(posX, posY, this->width(), this->height());
    const int offset = 20;

    for (NoteWidget *existingNote : existingNotes) {
        QRect existingNoteRect = existingNote->geometry();
        if (newNoteRect.intersects(existingNoteRect)) {
            posY += existingNoteRect.height() + offset;
            newNoteRect.moveTo(posX, posY);
            if (posY + this->height() > screenGeometry.bottom()) {
                posY = 20;
            }
        }
    }

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
}

void NoteWidget::saveNote() {
    if (filePath.isEmpty()) {
        return;
    }

    QString noteTitle = ui->noteTitleLineEdit->text();
    QString noteContent = ui->noteTextEdit->toHtml();
    int fontSize = ui->noteTextEdit->font().pointSize();

    QJsonObject noteObject;
    noteObject["title"] = noteTitle;
    noteObject["content"] = noteContent;
    noteObject["fontSize"] = fontSize;
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

            ui->noteTextEdit->setHtml(noteContent);
            QStringList lines = ui->noteTextEdit->toPlainText().split('\n');
            bool allHaveBullets = std::all_of(lines.begin(), lines.end(), [](const QString &line) {
                return line.trimmed().startsWith("•");
            });

            if (allHaveBullets) {
                ui->bulletlistButton->setChecked(true);
                ui->bulletlistButton->setIcon(getIcon(11, true));
                convertToBulletList();
            }

            int fontSize = noteObject.value("fontSize").toInt(11);
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
    connect(this, &NoteWidget::closed, qwoteInstance, [this]() {
        qwoteInstance->onNoteDeleted(this);
    });
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(250);
    animation->setStartValue(1);
    animation->setEndValue(0);

    connect(animation, &QPropertyAnimation::finished, this, [this, animation]() {
        animation->deleteLater();
        if (!filePath.isEmpty()) {
            QFile::remove(filePath);
        }
        existingNotes.removeAll(this);
        emit closed();
        this->deleteLater();
    });

    animation->start();
}

void NoteWidget::createNewNote() {
    if (qwoteInstance) {
        qwoteInstance->createNewNote();
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

    backgroundColor.setAlpha(opacity);
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::transparent);
    if (roundedCorners) {
        painter.drawRoundedRect(rect(), 8, 8);
    } else {
        painter.drawRect(rect());
    }
}

void NoteWidget:: setButtons() {
    ui->newButton->setIcon(getIcon(1, false));
    ui->pinButton->setIcon(getIcon(2, false));
    ui->closeButton->setIcon(getIcon(3, false));
}

void NoteWidget::setTitle() {
#ifdef _WIN32
    ui->noteTitleLineEdit->setPalette(setTitleColor(ui->noteTitleLineEdit->palette()));
#endif
    ui->noteTitleLineEdit->setPlaceholderText(getRandomPlaceholder());
}

void NoteWidget::mousePressEvent(QMouseEvent *event) {
    if (ctrlPressed && event->button() == Qt::MiddleButton) {
        resetFontSize();
    }
    if (event->button() == Qt::LeftButton) {
        if (resizeDirection != Qt::Edges()) {
            isResizing = true;
            this->grabMouse();
        } else {
            isDragging = true;
            dragStartPosition = event->pos();
            this->setMouseTracking(true);
        }
    }
}

void NoteWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint newPos = this->pos() + (event->pos() - dragStartPosition);
        this->move(newPos);
    } else if (isResizing) {
        QRect currentGeometry = this->geometry();
        QPoint globalMousePos = event->globalPosition().toPoint();

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
        updateCursorShape(event->pos());
    }
}

void NoteWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizing = false;
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

    resizeDirection = detectedEdges;
}

void NoteWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) {
        ctrlPressed = true;
    }
    QWidget::keyPressEvent(event);
}

void NoteWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) {
        ctrlPressed = false;
    }
    QWidget::keyReleaseEvent(event);
}

void NoteWidget::wheelEvent(QWheelEvent *event) {
    if (ctrlPressed) {
        if (event->angleDelta().y() > 0) {
            increaseFontSize();
        } else if (event->angleDelta().y() < 0) {
            decreaseFontSize();
        } else if (event->buttons() & Qt::MiddleButton) {
            resetFontSize();
        }
    } else {
        QWidget::wheelEvent(event);
    }
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
    QString defaultFontFamily;
#ifdef _WIN32
    defaultFontFamily = "Consolas";
#elif __linux__
    defaultFontFamily = "Monospace";
#endif

    QSettings settings("Odizinne", "Qwote");

    QFont font;
    font.setFamily(settings.value("font", defaultFontFamily).toString());
    font.setPointSize(settings.value("fontSize", 11).toInt());
    ui->noteTextEdit->setFont(font);

    QFont titleFont = font;
    titleFont.setPointSize(settings.value("titleFontSize", 11).toInt());
    titleFont.setBold(true);
    ui->noteTitleLineEdit->setFont(titleFont);

    opacity = settings.value("opacity", 255).toInt();
    roundedCorners = settings.value("roundedCorners", true).toBool();

    setContentTransparency();
    setTitleTransparency();
    update();
}

void NoteWidget::updateFormat() {
    QTextCharFormat format;
    QTextCursor cursor = ui->noteTextEdit->textCursor();

    if (ui->boldButton->isChecked()) {
        format.setFontWeight(QFont::Bold);
    } else {
        format.setFontWeight(QFont::Normal);
    }

    if (ui->italicButton->isChecked()) {
        format.setFontItalic(true);
    } else {
        format.setFontItalic(false);
    }

    if (ui->underlineButton->isChecked()) {
        format.setFontUnderline(true);
    } else {
        format.setFontUnderline(false);
    }

    if (ui->strikethroughButton->isChecked()) {
        format.setFontStrikeOut(true);
    } else {
        format.setFontStrikeOut(false);
    }

    cursor.mergeCharFormat(format);
    ui->noteTextEdit->setTextCursor(cursor);
}

void NoteWidget::setTextEditButtons() {
    ui->boldButton->setIcon(getIcon(4, false));
    ui->italicButton->setIcon(getIcon(5, false));
    ui->plusButton->setIcon(getIcon(6, false));
    ui->minusButton->setIcon(getIcon(7, false));
    ui->editorToolsButton->setIcon(getIcon(8, false));
    ui->underlineButton->setIcon(getIcon(9, false));
    ui->strikethroughButton->setIcon(getIcon(10, false));
    ui->bulletlistButton->setIcon(getIcon(11, false));
}

void NoteWidget::onBoldButtonStateChanged() {
    ui->boldButton->setIcon(getIcon(4, ui->boldButton->isChecked()));
    updateFormat();
}

void NoteWidget::onItalicButtonStateChanged() {
    ui->italicButton->setIcon(getIcon(5, ui->italicButton->isChecked()));
    updateFormat();
}

void NoteWidget::onUnderlineButtonStateChanged() {
    ui->underlineButton->setIcon(getIcon(9, ui->underlineButton->isChecked()));
    updateFormat();
}

void NoteWidget::onStrikethroughButtonStateChanged() {
    ui->strikethroughButton->setIcon(getIcon(10, ui->strikethroughButton->isChecked()));
    updateFormat();
}

void NoteWidget::onEditorToolsButtonStateChanged() {
    bool state = ui->editorToolsButton->isChecked();
    ui->frame->setVisible(state);
    ui->editorToolsButton->setIcon(getIcon(8, state));
}

void NoteWidget::onBulletListButtonStateChanged() {
    bool state = ui->bulletlistButton->isChecked();
    if (state) {
        convertToBulletList();
    } else {
        revertBulletList();
    }
    ui->bulletlistButton->setIcon(getIcon(11, state));
}

void NoteWidget::addBulletOnNewLine() {
    QTextCursor cursor = ui->noteTextEdit->textCursor();

    if (cursor.atBlockEnd() && cursor.block().text().isEmpty()) {
        cursor.insertText("• ");
    }
}

void NoteWidget::convertToBulletList() {
    QTextCursor cursor = ui->noteTextEdit->textCursor();
    int cursorPosition = cursor.position();

    QStringList lines = ui->noteTextEdit->toPlainText().split('\n');

    QStringList modifiedLines;
    bool hasChanges = false;

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.startsWith("•")) {
            modifiedLines << QString("• %1").arg(trimmedLine);
            hasChanges = true;
        } else {
            modifiedLines << trimmedLine;
        }
    }

    if (hasChanges) {
        ui->noteTextEdit->setPlainText(modifiedLines.join('\n'));
    }

    cursor.setPosition(qMin(cursorPosition, ui->noteTextEdit->toPlainText().length()));
    ui->noteTextEdit->setTextCursor(cursor);

    connect(ui->noteTextEdit, &QTextEdit::textChanged, this, &NoteWidget::addBulletOnNewLine);
}


void NoteWidget::revertBulletList() {
    QTextCursor cursor = ui->noteTextEdit->textCursor();
    int cursorPosition = cursor.position();

    QStringList lines = ui->noteTextEdit->toPlainText().split('\n');
    QStringList unbulletedLines;

    for (const QString &line : lines) {
        QString unbulletedLine = line.startsWith("•") ? line.mid(2).trimmed() : line;
        unbulletedLines << unbulletedLine;
    }

    ui->noteTextEdit->setPlainText(unbulletedLines.join('\n'));

    cursor.setPosition(qMin(cursorPosition, ui->noteTextEdit->toPlainText().length()));
    ui->noteTextEdit->setTextCursor(cursor);

    disconnect(ui->noteTextEdit, &QTextEdit::textChanged, this, &NoteWidget::addBulletOnNewLine);
}

void NoteWidget::setTitleTransparency() {

    QPalette titlePalette = ui->noteTitleLineEdit->palette();

    QColor actualBaseColor = titlePalette.color(QPalette::Base);
    actualBaseColor.setAlpha(opacity);
    titlePalette.setColor(QPalette::Base, actualBaseColor);

    ui->noteTitleLineEdit->setPalette(titlePalette);
}

void NoteWidget::setContentTransparency() {
    QPalette contentPalette = ui->noteTextEdit->palette();

    QColor actualBaseColor = contentPalette.color(QPalette::Base);
    actualBaseColor.setAlpha(opacity);
    contentPalette.setColor(QPalette::Base, actualBaseColor);

    ui->noteTextEdit->setPalette(contentPalette);
}

