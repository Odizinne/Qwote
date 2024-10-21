#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>
#include <QJsonObject>

class Qwote;

namespace Ui {
class NoteWidget;
}

class NoteWidget : public QWidget {
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent, const QString &filePath, bool restored, Qwote *qwoteInstance = nullptr);
    ~NoteWidget();

    void setNoteTitle(const QString &title);
    void setNoteContent(const QString &content);
    void deleteNote();
    void createNewNote();
    void savePosition();
    void restorePosition(const QPoint &position);
    static QList<NoteWidget*> existingNotes;
    void loadSettings();

private slots:
    void togglePinnedState();
    void onNoteTitleChanged();

private:
    Qwote* qwoteInstance;
    Ui::NoteWidget *ui;
    bool isDragging;
    QString filePath;
    bool isRestored;
    QPoint dragStartPosition;
    bool isPinned;
    void setTitle();
    void placeNote();
    void fadeIn();
    void updateCursorShape(const QPoint &pos);
    bool isResizing = false;
    Qt::Edges resizeDirection = Qt::Edges();
    void setTextEditFontSize(int fontSize);
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();
    QJsonObject settings;
    static const QString settingsFile;
    bool ctrlPressed;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void createNewNoteFile();
    void loadNoteFromFile();
    void saveNote();

signals:
    void closed();
};

#endif // NOTEWIDGET_H
