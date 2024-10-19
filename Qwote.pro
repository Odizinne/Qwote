QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    src/ \
    src/NoteWidget/ \
    src/Qwote/ \
    src/ShortcutManager/ \
    src/Utils/

SOURCES += \
    src/main.cpp \
    src/NoteWidget/notewidget.cpp \
    src/Qwote/qwote.cpp \
    src/ShortcutManager/shortcutmanager.cpp \
    src/Utils/utils.cpp

HEADERS += \
    src/NoteWidget/notewidget.h \
    src/Qwote/qwote.h \
    src/ShortcutManager/shortcutmanager.h \
    src/Utils/utils.h

FORMS += \
    src/NoteWidget/notewidget.ui

RESOURCES += \
    src/Resources/resources.qrc \

RC_FILE = src/Resources/appicon.rc

LIBS += -luser32 -ladvapi32 -lwinmm

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
