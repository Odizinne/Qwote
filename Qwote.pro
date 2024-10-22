QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH +=                                  \
    src/                                        \
    src/NoteWidget/                             \
    src/Qwote/                                  \
    src/QwoteServer/                            \
    src/SettingsPage/                           \
    src/ShortcutManager/                        \
    src/Utils/                                  \

SOURCES +=                                      \
    src/main.cpp                                \
    src/NoteWidget/notewidget.cpp               \
    src/Qwote/qwote.cpp                         \
    src/QwoteServer/qwoteserver.cpp             \
    src/SettingsPage/settingspage.cpp           \
    src/ShortcutManager/shortcutmanager.cpp     \
    src/Utils/utils.cpp                         \

HEADERS +=                                      \
    src/NoteWidget/notewidget.h                 \
    src/Qwote/qwote.h                           \
    src/QwoteServer/qwoteserver.h               \
    src/SettingsPage/settingspage.h             \
    src/ShortcutManager/shortcutmanager.h       \
    src/Utils/utils.h                           \

FORMS +=                                        \
    src/NoteWidget/notewidget.ui                \
    src/SettingsPage/settingspage.ui            \

RESOURCES +=                                    \
    src/resources/resources.qrc                 \

RC_FILE = src/resources/appicon.rc


win32 {
    LIBS += -luser32 -ladvapi32
}

TRANSLATIONS +=                                 \
    src/resources/tr/Qwote_en.ts                \
    src/resources/tr/Qwote_fr.ts                \

CONFIG += lrelease
QM_FILES_RESOURCE_PREFIX=/translations/tr
CONFIG += embed_translations
