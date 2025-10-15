QT       += widgets
CONFIG   += c++20
macx: CONFIG -= app_bundle

TEMPLATE = app
TARGET   = tetris_qt

# Директория для объектников
OBJECTS_DIR = obj_qt
MOC_DIR = obj_qt

# исходники GUI тетриса (положи сюда свои файлы)
SOURCES += \
    $$PWD/TetrisController.cpp \
    $$PWD/TetrisWidget.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/TetrisController.h \
    $$PWD/TetrisWidget.h

# инклуды на C-бэкенд тетриса + общий корень
INCLUDEPATH += \
    $$PWD/../../../ \
    $$PWD/../../../brick_game/tetris/include

# линковка с твоей C-библиотекой тетриса
LIBS += -L$$PWD/../../../lib -ltetris