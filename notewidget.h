#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class NoteWidget; }
QT_END_NAMESPACE

class NoteWidget : public QWidget {
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent = nullptr);
    ~NoteWidget();
    static void createNewNote();

signals:
    void noteClosed();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::NoteWidget *ui;
    bool isDragging;
    QPoint dragStartPosition;
};

#endif // NOTEWIDGET_H
