INCLUDEPATH += include \
               BD5/include

DEPENDPATH += src \
              BD5/src

HEADERS       = include/glwidget.h \
                include/window.h \
                include/mainwindow.h \
                BD5/include/BD5File.h \
                BD5/include/DataSet.h \
                BD5/include/Group.h \
                BD5/include/Object.h \
                BD5/include/ScaleUnit.h \
                BD5/include/Snapshot.h \
                BD5/include/TypeDescriptor.h \
                BD5/include/Logger.h \
                BD5/include/utils.h
SOURCES       = src/glwidget.cpp \
                src/window.cpp \
                src/mainwindow.cpp \
                src/main.cpp \
                BD5/src/BD5File.cpp \
                BD5/src/DataSet.cpp \
                BD5/src/Group.cpp \
                BD5/src/Object.cpp \
                BD5/src/ScaleUnit.cpp \
                BD5/src/Snapshot.cpp \
                BD5/src/TypeDescriptor.cpp \
                BD5/src/Logger.cpp

QT += widgets opengl openglwidgets

DESTDIR=bin
OBJECTS_DIR=build
MOC_DIR=build

# On mac the HDF5 library was compilated from source code, modify for Homebrew or other installation method
macx: {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
    INCLUDEPATH += /usr/local/HDF_Group/HDF5/1.12.2/include
    INCLUDEPATH += /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/GLUT.framework/Headers
    LIBS += -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks -framework GLUT
    LIBS += -L/usr/local/HDF_Group/HDF5/1.12.2/lib -lhdf5 -lhdf5_cpp
}

# Linux
unix:!macx {
    INCLUDEPATH += /usr/include/hdf5/serial
    LIBS += -L /usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5 -lhdf5_cpp -lGLU -lglut
}

# On windows HDF5 library was compilated  from source code with Visual Studio 2022, modify for other installation method
win32 {
    INCLUDEPATH += "C:\\Program Files\\HDF_Group\\HDF5\\1.12.0\\include"
    LIBS += "C:\\Program Files\\HDF_Group\\HDF5\\1.12.0\\lib" -lhdf5 -lhdf5_cpp
}

# install
target.path = ./BD5Viewer
INSTALLS += target
