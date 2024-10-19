#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>

namespace Ui {
class NoteWidget;
}

class NoteWidget : public QWidget {
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent = nullptr, const QString &filePath = QString(), bool restored = false);
    ~NoteWidget();

    void setNoteTitle(const QString &title);
    void setNoteContent(const QString &content);
    void deleteNote();
    static void createNewNote();

    // New methods for saving and restoring position
    void savePosition();
    void restorePosition(const QPoint &position);

private:
    Ui::NoteWidget *ui;
    bool isDragging;
    QString filePath;
    bool isRestored;
    QPoint dragStartPosition;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void createNewNoteFile();
    void loadNoteFromFile();
    void saveNote();
};

#endif // NOTEWIDGET_H
