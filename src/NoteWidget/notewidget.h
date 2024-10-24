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

    static QList<NoteWidget*> existingNotes;
    void setNoteTitle(const QString &title);
    void setNoteContent(const QString &content);
    void restorePosition(const QPoint &position);
    void deleteNote();
    void createNewNote();
    void savePosition();
    void loadSettings();

private slots:
    void togglePinnedState();
    void onNoteTitleChanged();
    void updateFormat();
    void onBoldButtonStateChanged();
    void onItalicButtonStateChanged();
    void onUnderlineButtonStateChanged();
    void onEditorToolsButtonStateChanged();
    void onStrikethroughButtonStateChanged();
    void onBulletListButtonStateChanged();

private:
    Qwote* qwoteInstance;
    Ui::NoteWidget *ui;
    QString filePath;
    bool isDragging;
    bool isRestored;
    bool isPinned;
    bool isResizing;
    bool ctrlPressed;
    Qt::Edges resizeDirection;
    QPoint dragStartPosition;
    QJsonObject settings;
    static const QString settingsFile;
    int opacity;
    bool roundedCorners;

    void setTextEditFontSize(int fontSize);
    void setTextEditButtons();
    void setTitleTransparency();
    void setContentTransparency();
    void setTitle();
    void placeNote();
    void fadeIn();
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();
    void updateCursorShape(const QPoint &pos);
    void createNewNoteFile();
    void loadNoteFromFile();
    void saveNote();
    void setButtons();
    void addBulletOnNewLine();
    void convertToBulletList();
    void revertBulletList();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void closed();
};

#endif // NOTEWIDGET_H
