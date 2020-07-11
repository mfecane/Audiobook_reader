QT -= gui
QT += multimedia

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        audiofilestream.cpp \
        main.cpp \
        player.cpp \
        player2.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -L"C:\dev\lib\soundtouch\source\SoundTouchDLL\x64\Debug" -lSoundTouchDLLD_x64
INCLUDEPATH += ..\soundtouch_dll-2.1.1


LIBS += -L"C:\dev\lib\bass24\c\x64" -Lbass
INCLUDEPATH += C:\dev\lib\bass24\c

HEADERS += \
    audiofilestream.h \
    player.h \
    player2.h


#LIBS += "C:\dev\lib\nsound-0.9.4\src\Nsound\nsound.lib"

#INCLUDEPATH += C:\dev\lib\nsound-0.9.4\src\Nsound
#INCLUDEPATH += C:\dev\lib\nsound-0.9.4\src\
