TEMPLATE = app
TARGET = nanonoise
DEPENDPATH += .
INCLUDEPATH += .
QT += xml widgets
#CONFIG += debug

win32:QMAKE_DEL_FILE = del /q
win32:QMAKE_DEL_DIR = rmdir /s /q

unix:QMAKE_DEL_FILE = /bin/rm -rf
unix:QMAKE_DEL_DIR = /bin/rm -rf

mac {
    DEFINES += __MACOSX_CORE__
    LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation
    CONFIG += x86 ppc
    ICON = images/icon.icns
}

unix:!mac {
    QT += x11extras
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
    RCC_DIR = .rcc
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa x11 xtst
    nanonoise-binary.path = /usr/bin
    nanonoise-binary.files = nanonoise
    nanonoise-icons.path = /usr/share/pixmaps
    nanonoise-icons.files = images/nanonoise.xpm
    nanonoise-desktop.path = /usr/share/applications
    nanonoise-desktop.files = nanonoise.desktop

    INSTALLS += nanonoise-binary nanonoise-icons nanonoise-desktop
}

windows { 
    DEFINES += __WINDOWS_MM__
    LIBS += -lwinmm
    RC_FILE = nanonoise.rc
}

HEADERS += X11DoTool.h \
    RtMidi.h \
    RenoiseKeyBindings.h \
    RenoiseKeyBinding.h \
    RenoiseCategory.h \
    MidiIO.h \
    MidiEvent.h \
    MidiKorgNanoKONTROL.h \
    SendKeys.h
SOURCES += X11DoTool.cpp \
    RtMidi.cpp \
    RenoiseKeyBindings.cpp \
    RenoiseKeyBinding.cpp \
    RenoiseCategory.cpp \
    MidiIO.cpp \
    MidiEvent.cpp \
    MidiKorgNanoKONTROL.cpp \
    main.cpp \
    SendKeys.cpp
FORMS += 
RESOURCES += resources.qrc

QMAKE_DISTCLEAN += object_script.* .ui .moc .rcc .obj
