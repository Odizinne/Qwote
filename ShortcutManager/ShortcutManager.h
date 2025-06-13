#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QString>

namespace ShortcutManager {

    bool isShortcutPresent();
    void manageShortcut(bool state);
    bool isDesktopfilePresent();
    void manageDesktopFile(bool state);
}

#endif // SHORTCUTMANAGER_H
