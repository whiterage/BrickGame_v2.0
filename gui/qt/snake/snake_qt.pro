QT       += widgets
CONFIG   += c++20
# убери следующую строку, если хочешь именно .app-бандл на macOS
macx: CONFIG -= app_bundle

TEMPLATE = app
TARGET   = snake_qt

# Директория для объектников
OBJECTS_DIR = obj_qt
MOC_DIR = obj_qt

# исходники GUI змейки
SOURCES += \
    $$PWD/GameController.cpp \
    $$PWD/SnakeWidget.cpp \
    $$PWD/SidebarWidget.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/GameController.h \
    $$PWD/SnakeWidget.h \
    $$PWD/SidebarWidget.h

# инклуды на твою модель
INCLUDEPATH += \
    $$PWD/../../../ \
    $$PWD/../../../brick_game/snake

# линкуем статическую библиотеку змейки
LIBS += -L$$PWD/../../../lib -lsnake