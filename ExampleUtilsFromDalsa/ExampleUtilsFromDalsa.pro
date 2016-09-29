TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    exampleutils.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += $$quote(C:\Program Files\Teledyne DALSA\Sapera\Classes\Basic)
INCLUDEPATH += $$quote(C:\Program Files\Teledyne DALSA\Sapera\Include)

LIBS +=$$quote(-LC:\Program Files\Teledyne DALSA\Sapera\Lib\Win64)\
                -lSapClassBasic

#win32:CONFIG(release, debug|release): LIBS += -L$$quote(PWD/../../../../../Program Files/Teledyne DALSA/Sapera/Lib/Win64)/ -lSapClassBasic
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$quote(PWD/../../../../../Program Files/Teledyne DALSA/Sapera/Lib/Win64)/ -lSapClassBasic


INCLUDEPATH += $$PWD/../../../../../Program Files/Teledyne DALSA/Sapera/Lib/Win64
DEPENDPATH += $$PWD/../../../../../Program Files/Teledyne DALSA/Sapera/Lib/Win64

HEADERS += \
    exampleutils.h

