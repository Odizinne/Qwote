#include "qwote.h"
#include "notewidget.h"
#include "shortcutmanager.h"
#include <QMenu>
#include <QApplication>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

using namespace ShortcutManager;

Qwote::Qwote(QWidget *parent)
    : QWidget{parent}
    , trayIcon(new QSystemTrayIcon(this))
{
    createTrayIcon();
    if (!restoreSavedNotes()) {
        createNewNote();
    }
}

Qwote::~Qwote() {
    qDeleteAll(noteWidgets);
    noteWidgets.clear();
}

void Qwote::createTrayIcon()
{
    trayIcon->setIcon(QIcon(":/icons/postit_icon.png"));
    QMenu *trayMenu = new QMenu(this);

    QAction *newAction = new QAction("New note", this);
    connect(newAction, &QAction::triggered, this, &Qwote::createNewNote);
    trayMenu->addAction(newAction);

    startupAction = new QAction("Start with system", this);
    startupAction->setCheckable(true);
    startupAction->setChecked(isShortcutPresent());
    connect(startupAction, &QAction::triggered, this, &Qwote::onStartupActionStateChanged);
    trayMenu->addAction(startupAction);

    trayMenu->addSeparator();

    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("QMS");
    trayIcon->show();
}

void Qwote::createNewNote() {
    NoteWidget *newNote = new NoteWidget(nullptr, QString(), false); // No parent
    noteWidgets.append(newNote);
}

bool Qwote::restoreSavedNotes() {
    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataLocation);
    if (!dir.exists()) {
        return false; // No notes directory exists
    }

    // List all JSON files following the pattern "note-*.json"
    QStringList noteFiles = dir.entryList(QStringList() << "note-*.json", QDir::Files);
    if (noteFiles.isEmpty()) {
        return false; // No note files found
    }

    // Iterate through each note file
    foreach (const QString &noteFile, noteFiles) {
        QString filePath = appDataLocation + "/" + noteFile;

        // Create a new NoteWidget and populate it with the data
        NoteWidget *noteWidget = new NoteWidget(nullptr, filePath, true); // Pass file path for restored notes
        noteWidgets.append(noteWidget);
    }

    return true;
}

void Qwote::onStartupActionStateChanged() {
    manageShortcut(startupAction->isChecked());
}
