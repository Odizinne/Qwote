#include "NoteWidget.h"
#include "ui_NoteWidget.h" // Auto-generated from the .ui file by uic
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>

NoteWidget::NoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteWidget),
    isDragging(false)
{
    ui->setupUi(this);
    ui->noteTitleLineEdit->setPlaceholderText("Name this note to save it");

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowMinimizeButtonHint);

    connect(ui->closeButton, &QToolButton::clicked, this, &NoteWidget::close);
    connect(ui->minimizeButton, &QToolButton::clicked, this, &NoteWidget::showMinimized);
    connect(ui->newButton, &QToolButton::clicked, this, &NoteWidget::createNewNote);
}

NoteWidget::~NoteWidget() {
    delete ui;
}

void NoteWidget::createNewNote() {
    NoteWidget *newNote = new NoteWidget();  // Create a new instance of NoteWidget
    newNote->show();  // Show the new note window
}

// Mouse event for dragging the window
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
    }
}
