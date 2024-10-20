#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>
#include <QJsonObject>

class Qwote;  // Forward declaration

namespace Ui {
class NoteWidget;  // Forward declare the UI class
}

class NoteWidget : public QWidget {
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent, const QString &filePath, bool restored, Qwote *qwoteInstance = nullptr);
    ~NoteWidget();

    // Method declarations...
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
    Qwote* qwoteInstance;  // Pointer to Qwote
    Ui::NoteWidget *ui;
    bool isDragging;
    QString filePath;
    bool isRestored;
    QPoint dragStartPosition;
    bool isPinned;
    void setTitleColor();
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

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void createNewNoteFile();
    void loadNoteFromFile();
    void saveNote();
};

#endif // NOTEWIDGET_H
