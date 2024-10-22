#include "shortcutmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QShortcut>
#include <QStandardPaths>

#ifdef _WIN32
void ShortcutManager::manageShortcut(bool state)
{
    QString shortcutName = "Qwote.lnk";
    QString applicationPath = QCoreApplication::applicationFilePath();
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator() + "Startup";
    QString shortcutPath = startupPath + QDir::separator() + shortcutName;

    if (state) {
        QFile::link(applicationPath, shortcutPath);
    } else {
        QFile::remove(shortcutPath);
    }
}

bool ShortcutManager::isShortcutPresent()
{
    QString shortcutName = "Qwote.lnk";
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator() + "Startup";
    QString shortcutPath = startupPath + QDir::separator() + shortcutName;

    return QFile::exists(shortcutPath);
}
#endif

#ifdef __linux__
const QString desktopFile = QDir::homePath() + "/.config/autostart/qwote.desktop";

bool ShortcutManager::isDesktopfilePresent()
{
    if (QFile::exists(desktopFile)) {
        return true;
    }
    return false;
}

void createDesktopFile()
{

    QFileInfo fileInfo(desktopFile);
    QDir dir = fileInfo.dir();

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString applicationFolder = QCoreApplication::applicationDirPath();
    QString desktopEntryContent =
        "[Desktop Entry]\n"
        "Path=" + applicationFolder + "\n"
                              "Type=Application\n"
                              "Exec=" + QCoreApplication::applicationFilePath() + "\n"
                                                    "Name=Qwote\n";

    QFile file(desktopFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << desktopEntryContent;
        file.close();
    }
}

void removeDesktopFile()
{
    QFile file(desktopFile);
    if (file.exists()) {
        file.remove();
    }
}

void ShortcutManager::manageDesktopFile(bool state)
{
    if (state) {
        createDesktopFile();
    } else {
        removeDesktopFile();
    }
}
#endif
