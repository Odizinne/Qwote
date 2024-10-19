#include "notewidget.h"
#include "ui_notewidget.h"
#include <QMouseEvent>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QTextStream>
#include <QPainter>

NoteWidget::NoteWidget(QWidget *parent, const QString &filePath, bool restored)
    : QWidget(parent),
    ui(new Ui::NoteWidget),
    isDragging(false),
    filePath(filePath),
    isRestored(restored),
    isPinned(false)
{
    ui->setupUi(this);
    ui->noteTitleLineEdit->setPlaceholderText("Name this note");
    setWindowTitle("New note");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    if (isRestored) {
        loadNoteFromFile();
    } else {
        createNewNoteFile();
    }

    // Save whenever the text or title changes
    connect(ui->closeButton, &QToolButton::clicked, this, &NoteWidget::deleteNote);
    connect(ui->pinButton, &QToolButton::clicked, this, &NoteWidget::togglePinnedState);
    connect(ui->newButton, &QToolButton::clicked, this, &NoteWidget::createNewNote);
    connect(ui->noteTextEdit, &QTextEdit::textChanged, this, &NoteWidget::saveNote);
    connect(ui->noteTitleLineEdit, &QLineEdit::textChanged, this, &NoteWidget::onNoteTitleChanged);
}

NoteWidget::~NoteWidget() {
    delete ui;
}

void NoteWidget::togglePinnedState() {
    isPinned = ui->pinButton->isChecked();
    setWindowFlags(isPinned ? Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint : Qt::Window | Qt::FramelessWindowHint);
    show();
    saveNote();
}

void NoteWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void NoteWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - dragStartPosition);
        event->accept();
    }
}

void NoteWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        savePosition();  // Save the position when the mouse is released
    }
}

void NoteWidget::createNewNoteFile() {
    // Get the AppData location
    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(appDataLocation);
    if (!dir.exists()) {
        dir.mkpath(appDataLocation); // Ensure the directory exists
    }

    // Find a unique filename
    int i = 1;
    QString baseFileName;
    do {
        baseFileName = QString("note-%1.json").arg(i++);
    } while (QFile::exists(appDataLocation + "/" + baseFileName));

    // Set the file path for this note
    filePath = appDataLocation + "/" + baseFileName;

    // Save the empty note initially
    saveNote();
    savePosition(); // Save the initial position (0,0) by default
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
    noteObject["pinned"] = isPinned; // Save pinned state

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
            togglePinnedState();
        }
    }
}

void NoteWidget::setNoteTitle(const QString &title) {
    ui->noteTitleLineEdit->setText(title);
}

void NoteWidget::setNoteContent(const QString &content) {
    ui->noteTextEdit->setPlainText(content);
}

void NoteWidget::deleteNote() {
    if (!filePath.isEmpty()) {
        QFile::remove(filePath);
    }

    this->close();
}

void NoteWidget::createNewNote() {
    NoteWidget *newNote = new NoteWidget();
    newNote->show();
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

    // Get the background color from the parent widget or application style
    QColor backgroundColor;

    backgroundColor = this->palette().color(QPalette::Window);
    qDebug() << backgroundColor;

    painter.setBrush(backgroundColor);
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(rect(), 8, 8); // 5px radius for corners
}
