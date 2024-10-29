QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 silent lrelease embed_translations

QM_FILES_RESOURCE_PREFIX=/translations/tr

INCLUDEPATH +=                              \
                                            \
    NoteWidget/                             \
    Qwote/                                  \
    QwoteServer/                            \
    SettingsPage/                           \
    ShortcutManager/                        \
    Utils/                                  \

SOURCES +=                                  \
    main.cpp                                \
    NoteWidget/NoteWidget.cpp               \
    Qwote/Qwote.cpp                         \
    QwoteServer/QwoteServer.cpp             \
    SettingsPage/SettingsPage.cpp           \
    ShortcutManager/ShortcutManager.cpp     \
    Utils/Utils.cpp                         \

HEADERS +=                                  \
    NoteWidget/NoteWidget.h                 \
    Qwote/Qwote.h                           \
    QwoteServer/QwoteServer.h               \
    SettingsPage/SettingsPage.h             \
    ShortcutManager/ShortcutManager.h       \
    Utils/Utils.h                           \

FORMS +=                                    \
    NoteWidget/NoteWidget.ui                \
    SettingsPage/SettingsPage.ui            \

RESOURCES +=                                \
    resources/resources.qrc                 \


TRANSLATIONS +=                             \
    resources/tr/Qwote_en.ts                \
    resources/tr/Qwote_fr.ts                \

RC_FILE = resources/appicon.rc

win32 {
    LIBS += -luser32 -ladvapi32
}
