QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH +=                                  \
    src/                                        \
    src/NoteWidget/                             \
    src/Qwote/                                  \
    src/ShortcutManager/                        \
    src/Utils/                                  \
    src/SettingsPage/                           \
    src/QwoteServer/

SOURCES +=                                      \
    src/QwoteServer/qwoteserver.cpp \
    src/SettingsPage/settingspage.cpp           \
    src/main.cpp                                \
    src/NoteWidget/notewidget.cpp               \
    src/Qwote/qwote.cpp                         \
    src/ShortcutManager/shortcutmanager.cpp     \
    src/Utils/utils.cpp                         \

HEADERS +=                                      \
    src/NoteWidget/notewidget.h                 \
    src/Qwote/qwote.h                           \
    src/QwoteServer/qwoteserver.h \
    src/SettingsPage/settingspage.h             \
    src/ShortcutManager/shortcutmanager.h       \
    src/Utils/utils.h                           \

FORMS +=                                        \
    src/NoteWidget/notewidget.ui                \
    src/SettingsPage/settingspage.ui            \

RESOURCES +=                                    \
    src/Resources/resources.qrc                 \

RC_FILE = src/Resources/appicon.rc

LIBS += -luser32 -ladvapi32

