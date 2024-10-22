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
    , settingsPage(nullptr)
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

    QAction *newAction = new QAction(tr("New note"), this);
    connect(newAction, &QAction::triggered, this, &Qwote::createNewNote);
    trayMenu->addAction(newAction);

    startupAction = new QAction(tr("Start with system"), this);
    startupAction->setCheckable(true);
#ifdef _WIN32
    startupAction->setChecked(isShortcutPresent());
#elif __linux__
    startupAction->setChecked(isDesktopfilePresent());
#endif
    connect(startupAction, &QAction::triggered, this, &Qwote::onStartupActionStateChanged);
    trayMenu->addAction(startupAction);

    QAction *settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, &QAction::triggered, this, &Qwote::showSettings);
    trayMenu->addAction(settingsAction);

    trayMenu->addSeparator();

    QAction *deleteAllAction = new QAction(tr("Delete all notes"), this);
    connect(deleteAllAction, &QAction::triggered, this, &Qwote::deleteAllNotes);
    trayMenu->addAction(deleteAllAction);

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &Qwote::quitQwote);
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("Qwote");
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &Qwote::trayIconActivated);
}

void Qwote::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        createNewNote();
    }
}

void Qwote::createNewNote() {
    NoteWidget *newNote = new NoteWidget(nullptr, QString(), false, this);
    noteWidgets.append(newNote);

    if (settingsPage) {
        connect(settingsPage, &SettingsPage::fontChanged, newNote, &NoteWidget::loadSettings);
    }
}

bool Qwote::restoreSavedNotes() {
    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataLocation);
    if (!dir.exists()) {
        return false;
    }

    QStringList noteFiles = dir.entryList(QStringList() << "note-*.json", QDir::Files);
    if (noteFiles.isEmpty()) {
        return false;
    }

    foreach (const QString &noteFile, noteFiles) {
        QString filePath = appDataLocation + "/" + noteFile;

        NoteWidget *noteWidget = new NoteWidget(nullptr, filePath, true, this);
        noteWidgets.append(noteWidget);
    }

    return true;
}

void Qwote::onStartupActionStateChanged() {
#ifdef _WIN32
    manageShortcut(startupAction->isChecked());
#elif __linux__
    manageDesktopFile(startupAction->isChecked());
#endif
}

void Qwote::showSettings()
{
    if (settingsPage) {
        settingsPage->showNormal();
        settingsPage->raise();
        settingsPage->activateWindow();
        return;
    }

    settingsPage = new SettingsPage;
    settingsPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsPage, &SettingsPage::closed, this, &Qwote::onSettingsPageClosed);
    for (NoteWidget *noteWidget : noteWidgets) {
        connect(settingsPage, &SettingsPage::fontChanged, noteWidget, &NoteWidget::loadSettings);
    }
    settingsPage->show();
}

void Qwote::onSettingsPageClosed()
{
    settingsPage = nullptr;
}

void Qwote::deleteAllNotes() {
    for (NoteWidget *noteWidget : noteWidgets) {
        noteWidget->deleteNote();
    }
    noteWidgets.clear();
}

void Qwote::quitQwote()
{
    for (NoteWidget *noteWidget : noteWidgets) {
        disconnect(noteWidget, nullptr, this, nullptr);
        noteWidget->deleteLater();
    }
    noteWidgets.clear();
    QApplication::quit();
}

void Qwote::onNoteDeleted(NoteWidget *noteWidget) {
    noteWidgets.removeAll(noteWidget);
}
