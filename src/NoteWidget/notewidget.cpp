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

using namespace Utils;

const int RESIZE_MARGIN = 7;

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
    setMouseTracking(true);

    ui->newButton->setIcon(getIcon(1, false));
    ui->pinButton->setIcon(getIcon(2, false));
    ui->closeButton->setIcon(getIcon(3, false));

    QPalette palette = ui->noteTitleLineEdit->palette();
    palette.setColor(QPalette::Text, getAccentColor("normal"));
    ui->noteTitleLineEdit->setPalette(palette);

    if (isRestored) {
        loadNoteFromFile();
    } else {
        createNewNoteFile();
    }

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
    setWindowFlag(Qt::WindowStaysOnTopHint, isPinned);
    ui->pinButton->setIcon(getIcon(2, isPinned));
    show();
    saveNote();
}

void NoteWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QRect geom = geometry();
        QPoint globalPos = event->globalPosition().toPoint();

        if (abs(globalPos.x() - geom.right()) <= RESIZE_MARGIN || abs(globalPos.y() - geom.bottom()) <= RESIZE_MARGIN) {
            isDragging = false;  // Disable dragging when resizing
        } else {
            isDragging = true;  // Normal dragging
            dragStartPosition = globalPos - geom.topLeft();
        }
        event->accept();
    }
}

void NoteWidget::mouseMoveEvent(QMouseEvent *event) {
    QRect geom = geometry();
    QPoint globalPos = event->globalPosition().toPoint();
    int left = geom.left();
    int top = geom.top();
    int right = geom.right();
    int bottom = geom.bottom();

    bool nearLeft = abs(globalPos.x() - left) <= RESIZE_MARGIN;
    bool nearRight = abs(globalPos.x() - right) <= RESIZE_MARGIN;
    bool nearTop = abs(globalPos.y() - top) <= RESIZE_MARGIN;
    bool nearBottom = abs(globalPos.y() - bottom) <= RESIZE_MARGIN;

    // Change cursor based on mouse proximity to edges/corners
    if (nearLeft && nearTop) {
        setCursor(Qt::SizeFDiagCursor);  // Top-left corner
    } else if (nearRight && nearBottom) {
        setCursor(Qt::SizeFDiagCursor);  // Bottom-right corner
    } else if (nearLeft && nearBottom) {
        setCursor(Qt::SizeBDiagCursor);  // Bottom-left corner
    } else if (nearRight && nearTop) {
        setCursor(Qt::SizeBDiagCursor);  // Top-right corner
    } else if (nearRight) {
        setCursor(Qt::SizeHorCursor);    // Right edge
    } else if (nearLeft) {
        setCursor(Qt::SizeHorCursor);    // Left edge
    } else if (nearTop) {
        setCursor(Qt::SizeVerCursor);    // Top edge
    } else if (nearBottom) {
        setCursor(Qt::SizeVerCursor);    // Bottom edge
    } else {
        setCursor(Qt::ArrowCursor);      // Default cursor when not near edges
    }

    // Handle resizing if the left button is pressed and we are near an edge
    if (event->buttons() & Qt::LeftButton) {
        QRect newGeom = geom;  // Create a new geometry for resizing
        if (nearRight) {
            newGeom.setRight(globalPos.x());
        }
        if (nearBottom) {
            newGeom.setBottom(globalPos.y());
        }
        if (nearLeft) {
            newGeom.setLeft(globalPos.x());
        }
        if (nearTop) {
            newGeom.setTop(globalPos.y());
        }

        // Only resize if we are near an edge
        if (nearRight || nearBottom || nearLeft || nearTop) {
            setGeometry(newGeom);  // Apply the new geometry (resize)
        } else {
            // Handle dragging movement (when not resizing)
            move(globalPos - dragStartPosition);
        }
        event->accept();
    }
}


void NoteWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false; // Disable dragging after releasing mouse button
        setCursor(Qt::ArrowCursor); // Restore default cursor
        savePosition(); // Save the resized position
    }
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

    saveNote();
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
    noteObject["width"] = width();    // Save the widget's width
    noteObject["height"] = height();  // Save the widget's height
    noteObject["pinned"] = isPinned;   // Save pinned state

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

            // Load width and height from the saved object and resize the widget
            int width = noteObject.value("width").toInt();
            int height = noteObject.value("height").toInt();
            resize(width, height); // Restore the size of the widget

            isPinned = noteObject.value("pinned").toBool();
            ui->pinButton->setChecked(isPinned);
            ui->pinButton->setIcon(getIcon(2, isPinned));
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

    QColor backgroundColor;
    backgroundColor = this->palette().color(QPalette::Window);

    painter.setBrush(backgroundColor);
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(rect(), 8, 8);
}
