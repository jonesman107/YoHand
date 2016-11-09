MAKEFILE = qMakefile
OBJECTS_DIR=generated_files #Intermediate object files directory
MOC_DIR=generated_files #Intermediate moc files directory
INCLUDEPATH += $$PWD/leap/include/ $$PWD/libpd/pure-data/src $$PWD/libpd/libpd_wrapper $$PWD/libpd/cpp
CONFIG      += c++11 console
QT          += widgets
QMAKE_CXXFLAGS += -ggdb

macx {
    LIBS += -L$$PWD/leap/lib/x64 -lLeap $$PWD/libpd/libs/*.dylib -framework CoreAudio -framework CoreFoundation -lpthread
    DEFINES += __MACOSX_CORE__
    CONFIG -= app_bundle
} else {
    LIBS += -L$$PWD/leap/lib/x64 -lLeap $$PWD/libpd/libs/*.so -lasound -lpthread
    DEFINES += __LINUX_ALSA__
}

HEADERS     = glwidget.h \
              graphics.h \
              widget.h \
              window.h \
              leapcontroller.h \
              leappd.h \
              rtaudio.h \
              audiowidget.h \
              leapmux.h \
              musictheory.h \
              module.h \
              effectsmodule.h \
              metronome.h \
    mellotronmodule.h \
    synthmodule.h \
    instrumentmodule.h

SOURCES     = glwidget.cpp \
              graphics.cpp \
              main.cpp \
              widget.cpp \
              window.cpp \
              leapcontroller.cpp \
              leappd.cpp \
              rtaudio.cpp \
              audiowidget.cpp \
              leapmux.cpp \
              musictheory.cpp \
              effectsmodule.cpp \
    mellotronmodule.cpp \
    synthmodule.cpp \
    instrumentmodule.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/2dpainting
INSTALLS += target
