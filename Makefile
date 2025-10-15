CXX := g++
CC  := gcc
CXXFLAGS     := -std=c++20 -Wall -Wextra -Werror -g -O2
CFLAGS       := -std=c11   -Wall -Wextra -Werror -g -O2
CXXFLAGS_COV := -std=c++20 -Wall -Wextra -g -O0 --coverage -fprofile-arcs -ftest-coverage
CFLAGS_COV   := -std=c11   -Wall -Wextra -g -O0 --coverage -fprofile-arcs -ftest-coverage


SRC_DIR := .
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib


LIB_SRC := brick_game/snake/backend.cpp
LIB_OBJ := $(LIB_SRC:%.cpp=$(OBJ_DIR)/%.o)
LIB     := $(LIB_DIR)/libsnake.a


TETRIS_SRC := \
  brick_game/tetris/backend/backend.c \
  brick_game/tetris/backend/shapes_back.c \
  brick_game/tetris/backend/api.c \
  brick_game/tetris/backend/scoreboard.c
TETRIS_OBJ := $(TETRIS_SRC:%.c=$(OBJ_DIR)/%.o)
TETRIS_LIB := $(LIB_DIR)/libtetris.a


TEST_SRC := tests/smoketest.cpp tests/best_test.cpp tests/food_test.cpp tests/level_test.cpp
TEST_OBJ := $(TEST_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_BIN := $(BIN_DIR)/test_snake


COV_OBJ_DIR := obj_cov
COV_BIN_DIR := bin_cov
COV_LIB_DIR := lib_cov
COV_LIB_OBJ := $(LIB_SRC:%.cpp=$(COV_OBJ_DIR)/%.o)
COV_LIB     := $(COV_LIB_DIR)/libsnake_cov.a
COV_TEST_OBJ:= $(TEST_SRC:%.cpp=$(COV_OBJ_DIR)/%.o)
COV_TEST_BIN:= $(COV_BIN_DIR)/test_snake_cov


GTEST_CFLAGS := $(shell pkg-config --cflags gtest 2>/dev/null)
GTEST_LDLIBS := $(shell pkg-config --libs   gtest 2>/dev/null)





QT_PREFIX := $(shell brew --prefix qt 2>/dev/null)
ifeq ($(QT_PREFIX),)
  $(warning Qt не найден через Homebrew. Установи: brew install qt)
  QT_PREFIX := /opt/homebrew/opt/qt
endif


QMAKE := $(QT_PREFIX)/bin/qmake


QT_DIR  := gui/qt
QT_PRO  := $(QT_DIR)/brickgame_qt.pro
QT_BIN  := $(BIN_DIR)/brickgame_qt


CONSOLE_DIR  := gui/console
CONSOLE_SRCS := $(wildcard $(CONSOLE_DIR)/*.cpp)
CONSOLE_OBJ  := $(CONSOLE_SRCS:%.cpp=$(OBJ_DIR)/%.o)
CONSOLE_BIN  := $(BIN_DIR)/snake_console


TETRIS_CONSOLE_DIR  := brick_game/tetris/frontend
TETRIS_CONSOLE_SRCS := $(wildcard $(TETRIS_CONSOLE_DIR)/*.c)
TETRIS_CONSOLE_OBJ  := $(TETRIS_CONSOLE_SRCS:%.c=$(OBJ_DIR)/%.o)
TETRIS_CONSOLE_BIN  := $(BIN_DIR)/tetris_console




all: lib tetris-lib test qt


$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/brick_game/snake \
	           $(OBJ_DIR)/brick_game/tetris/backend \
	           $(OBJ_DIR)/tests \
	           $(OBJ_DIR)/$(CONSOLE_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LIB_DIR):
	@mkdir -p $(LIB_DIR)


$(COV_OBJ_DIR):
	@mkdir -p $(COV_OBJ_DIR)/brick_game/snake $(COV_OBJ_DIR)/brick_game/tetris/backend $(COV_OBJ_DIR)/tests

$(COV_BIN_DIR):
	@mkdir -p $(COV_BIN_DIR)

$(COV_LIB_DIR):
	@mkdir -p $(COV_LIB_DIR)


lib: $(LIB)

$(LIB): $(LIB_OBJ) | $(LIB_DIR)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJ_DIR)/brick_game/snake/%.o: brick_game/snake/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@


tetris-lib: $(TETRIS_LIB)

$(TETRIS_LIB): $(TETRIS_OBJ) | $(LIB_DIR)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJ_DIR)/brick_game/tetris/backend/%.o: brick_game/tetris/backend/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@


test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) $(LIB) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LDLIBS) -lgtest_main -lpthread

$(OBJ_DIR)/tests/%.o: tests/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(GTEST_CFLAGS) -I. -c $< -o $@

run-test: test
	./$(TEST_BIN)


cov-lib: $(COV_LIB)

$(COV_LIB): $(COV_LIB_OBJ) | $(COV_LIB_DIR)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(COV_OBJ_DIR)/brick_game/snake/%.o: brick_game/snake/%.cpp | $(COV_OBJ_DIR)
	$(CXX) $(CXXFLAGS_COV) -I. -c $< -o $@

$(COV_OBJ_DIR)/brick_game/tetris/backend/%.o: brick_game/tetris/backend/%.c | $(COV_OBJ_DIR)
	$(CC) $(CFLAGS_COV) -I. -c $< -o $@

cov-test: $(COV_TEST_BIN)

$(COV_TEST_BIN): $(COV_TEST_OBJ) $(COV_LIB) | $(COV_BIN_DIR)
	$(CXX) $(CXXFLAGS_COV) $^ -o $@ $(GTEST_LDLIBS) -lgtest_main -lpthread

$(COV_OBJ_DIR)/tests/%.o: tests/%.cpp | $(COV_OBJ_DIR)
	$(CXX) $(CXXFLAGS_COV) $(GTEST_CFLAGS) -I. -c $< -o $@

gcov_report: cov-test
	@echo "Running Snake coverage tests..."
	./$(COV_TEST_BIN)
	@echo "Generating coverage report..."
	@mkdir -p coverage
	@gcov $(COV_OBJ_DIR)/brick_game/snake/backend.gcda -o $(COV_OBJ_DIR)/brick_game/snake/ >/dev/null 2>&1 || true
	@mv *.gcov coverage/ 2>/dev/null || true
	@gcovr --root . --filter="brick_game/" --html --html-details -o coverage/index.html --gcov-ignore-errors=no_working_dir_found || true
	@echo "Coverage -> coverage/index.html"

open-coverage: coverage/index.html
	@open coverage/index.html || xdg-open coverage/index.html || true


qt: $(QT_BIN)
	@echo "Qt BrickGame built."

run-qt: qt
	@./$(QT_BIN)


$(QT_BIN): $(QT_PRO) $(LIB) | $(BIN_DIR)
	@cd $(QT_DIR) && $(QMAKE) brickgame_qt.pro
	@cd $(QT_DIR) && make
	@cp $(QT_DIR)/brickgame_qt.app/Contents/MacOS/brickgame_qt $(QT_BIN)


console: $(CONSOLE_BIN)
	@echo "Console Snake built."

tetris-console: $(TETRIS_CONSOLE_BIN)
	@echo "Console Tetris built."

run-console: console
	@./$(CONSOLE_BIN)

run-tetris-console: tetris-console
	@./$(TETRIS_CONSOLE_BIN)

$(CONSOLE_BIN): $(CONSOLE_OBJ) $(LIB) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I. $^ -o $@

$(OBJ_DIR)/gui/console/%.o: gui/console/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

$(TETRIS_CONSOLE_BIN): $(TETRIS_CONSOLE_OBJ) $(TETRIS_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I. $^ -o $@ -lncurses

$(OBJ_DIR)/brick_game/tetris/frontend/%.o: brick_game/tetris/frontend/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -c $< -o $@


clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) \
	       $(COV_OBJ_DIR) $(COV_BIN_DIR) $(COV_LIB_DIR) \
	       coverage *.gcov *.gcda *.gcno
	@cd $(QT_DIR) && rm -f Makefile brickgame_qt && rm -rf obj_qt brickgame_qt.app
	@find . -name "*.o" -delete
	@find . -name "*.a" -delete
	@find . -name "Makefile" -not -path "./Makefile" -delete
	@find . -name "moc_*.cpp" -delete
	@find . -name "*.app" -type d -exec rm -rf {} + 2>/dev/null || true
	@rm -rf docs/html/ docs/latex/ docs/rtf/ docs/xml/
	@rm -rf dist/
	@rm -f Doxyfile.bak Doxyfile.log
	@echo "Очищено."

PREFIX ?= /usr/local
install: qt
	@install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib
	@install -m 0755 $(QT_BIN) $(DESTDIR)$(PREFIX)/bin/brickgame_qt || true
	@install -m 0644 $(LIB) $(DESTDIR)$(PREFIX)/lib/libsnake.a || true
	@echo "Installed to $(DESTDIR)$(PREFIX)"

uninstall:
	@rm -f $(DESTDIR)$(PREFIX)/bin/brickgame_qt
	@rm -f $(DESTDIR)$(PREFIX)/lib/libsnake.a
	@echo "Uninstalled from $(DESTDIR)$(PREFIX)"

dvi:
	@echo "Генерация документации с помощью Doxygen..."
	@doxygen Doxyfile
	@echo "Документация создана в docs/html/"
	@echo "Откройте docs/html/index.html для просмотра документации"

dist:
	@mkdir -p dist
	@git archive --format=tar.gz -o dist/brickgame-src.tar.gz HEAD
	@echo "Архив создан: dist/brickgame-src.tar.gz"

help:
	@echo "Доступные цели:"
	@echo "  all            - lib + tetris-lib + test + qt"
	@echo "  lib            - сборка статической библиотеки Snake"
	@echo "  tetris-lib     - сборка статической библиотеки Tetris (C)"
	@echo "  test           - сборка тестов"
	@echo "  run-test       - запуск тестов"
	@echo "  qt             - сборка Qt BrickGame с меню"
	@echo "  run-qt         - запуск Qt BrickGame"
	@echo "  console        - сборка консольной змейки"
	@echo "  run-console    - запуск консольной змейки"
	@echo "  tetris-console - сборка консольного тетриса"
	@echo "  run-tetris-console - запуск консольного тетриса"
	@echo "  gcov_report    - HTML отчет покрытия кода"
	@echo "  dvi            - генерация документации с Doxygen"
	@echo "  dist           - создание дистрибутивного архива"
	@echo "  clean          - удаление артефактов и документации"

.PHONY: all lib tetris-lib test run-test qt run-qt \
        console run-console tetris-console run-tetris-console \
        gcov_report open-coverage cov-lib cov-test clean install uninstall dvi dist help