TEMPLATE = app
TARGET   = brickgame_qt

QT += widgets gui core
CONFIG += c++20 console
# единые строгие флаги
QMAKE_CXXFLAGS += -std=c++20 -Wall -Wextra -Werror
QMAKE_CFLAGS   += -std=c11   -Wall -Wextra -Werror

# чтобы убрать предупреждение про разные версии macOS:
QMAKE_MACOSX_DEPLOYMENT_TARGET = 15.0

# Инклуды (относительно gui/qt/)
INCLUDEPATH += . \
    snake \
    tetris \
    ../../brick_game/snake \
    ../../brick_game/tetris \
    ../../brick_game/tetris/backend/include

# --- Главный файл приложения (твой общий main) ---
SOURCES += \
    main.cpp

# --- Snake (Qt + бэкенд C++) ---
SOURCES += \
    snake/GameController.cpp \
    snake/SnakeWidget.cpp \
    snake/SidebarWidget.cpp \
    ../../brick_game/snake/backend.cpp

HEADERS += \
    snake/GameController.h \
    snake/SnakeWidget.h \
    snake/SidebarWidget.h \
    ../../brick_game/snake/backend.h

# --- Tetris (Qt + C++ адаптер + C-ядро) ---
SOURCES += \
    tetris/TetrisController.cpp \
    tetris/TetrisWidget.cpp \
    ../../brick_game/tetris/backend/engine.cpp \
    ../../brick_game/tetris/backend/tetris_backend.c \
    ../../brick_game/tetris/backend/shapes_back.c \
    ../../brick_game/tetris/backend/api.c \
    ../../brick_game/tetris/backend/scoreboard.c

# Переименуем объектные файлы чтобы избежать конфликтов
OBJECTS_DIR = obj_qt

HEADERS += \
    tetris/TetrisController.h \
    tetris/TetrisWidget.h \
    ../../brick_game/tetris/backend/engine.h \
    ../../brick_game/tetris/backend/include/api.h \
    ../../brick_game/tetris/backend/include/tetris_types.h \
    ../../brick_game/tetris/backend/include/tetris_backend.h \
    ../../brick_game/tetris/backend/include/scoreboard.h