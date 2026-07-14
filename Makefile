# # Курсовая работа. Си базовый уровень. гр.Д01-134 Попов. В.Г
# # Сборка консольной и gui программы чтения csv файлов report
# # debug режим. команда:   make
# # realese режим. команда: make DEBUG=0
# # сборка GUI. команда:     make winreport

# Определение операционной системы
ifeq ($(OS),Windows_NT)
    IS_WINDOWS = 1
else
    IS_WINDOWS = 0
endif

# Имена исполняемых файлов
TARGET = report
WIN_TARGET = appreport
TEST_TARGET = test

# Исходные каталоги проекта
SRC_DIR = src
GUI_SRC_DIR = winreport/src
HEAD_DIR = $(SRC_DIR)/headers
GUI_HEAD_DIR = $(GUI_SRC_DIR)/headers
BUILD_BASE_DIR = build

# Каталоги для тестов
TEST_DIR = tests
TEST_HEAD_DIR = $(TEST_DIR)/headers
LIBS_DIR = libs

# FLTK настройки
FLTK_VERSION = 1.4.5
FLTK_ARCHIVE = fltk-$(FLTK_VERSION)-source.tar.gz
FLTK_URL = https://github.com/fltk/fltk/releases/download/release-$(FLTK_VERSION)/$(FLTK_ARCHIVE)
FLTK_PREFIX = $(CURDIR)/fltk-install
NPROC = 8

ifneq ($(IS_WINDOWS), 0)
    GUI_RES_DIR = winreport/res
endif

CC = gcc
CXX = g++
CSTANDARD = -std=gnu99
CXXSTANDARD = -std=c++20

# Компилятор ресурсов Windows
ifneq ($(IS_WINDOWS), 0)
    WINDRES = windres
endif

# Режим сборки по умолчанию (1 - Debug, 0 - Release)
DEBUG ?= 1

# Базовые флаги компилятора
COMMON_FLAGS = $(CSTANDARD) -Wall -Wextra -Wpedantic -I$(HEAD_DIR) -MMD -MP

# Настройка путей и флагов в зависимости от выбранного режима
ifeq ($(DEBUG), 1)
    CFLAGS = $(COMMON_FLAGS) -g -O0 -DDEBUG
    CXXFLAGS = $(CXXSTANDARD) -Wall -Wextra -Wpedantic -g -O0 -I$(TEST_HEAD_DIR) -I$(HEAD_DIR) -DDEBUG
    
    # FLTK флаги для Debug
    GUI_CXXFLAGS = $(CXXSTANDARD) -Wall -Wextra -Wpedantic -g -O0 \
                   -I$(GUI_HEAD_DIR) -I$(HEAD_DIR) -I$(FLTK_PREFIX)/include -DDEBUG
    
    LDFLAGS = -lm
    GUI_LDFLAGS = -L$(FLTK_PREFIX)/lib -static-libgcc -static-libstdc++ -lm
    
    # Каталоги для Debug режима
    BUILD_DIR   = $(BUILD_BASE_DIR)/debug
    OBJ_DIR     = $(BUILD_DIR)/obj
    GUI_OBJ_DIR = $(BUILD_DIR)/gui_obj
    EXECUTE_DIR = $(BUILD_DIR)/bin
    LIB_OBJ_DIR = $(BUILD_DIR)/$(LIBS_DIR)
    ifneq ($(IS_WINDOWS), 0)
        GUI_RES_OBJ_DIR = $(BUILD_DIR)/gui_res
    endif
else
    CFLAGS = $(COMMON_FLAGS) -O3 -flto -DNDEBUG
    CXXFLAGS = $(CXXSTANDARD) -Wall -Wextra -Wpedantic -O3 -flto -I$(TEST_HEAD_DIR) -I$(HEAD_DIR) -DNDEBUG
    
    # FLTK флаги для Release
    GUI_CXXFLAGS = $(CXXSTANDARD) -Wall -Wextra -Wpedantic -O3 -flto \
                   -I$(GUI_HEAD_DIR) -I$(HEAD_DIR) -I$(FLTK_PREFIX)/include -DNDEBUG
    
    LDFLAGS = -flto=auto -static-libgcc -static-libstdc++ -static -s -lm
    GUI_LDFLAGS = -L$(FLTK_PREFIX)/lib -flto -s -lm
    
    # Каталоги для Release режима
    BUILD_DIR   = $(BUILD_BASE_DIR)/release
    OBJ_DIR     = $(BUILD_DIR)/obj
    GUI_OBJ_DIR = $(BUILD_DIR)/gui_obj
    EXECUTE_DIR = $(BUILD_DIR)/bin
    LIB_OBJ_DIR = $(BUILD_DIR)/$(LIBS_DIR)
    ifneq ($(IS_WINDOWS), 0)
        GUI_RES_OBJ_DIR = $(BUILD_DIR)/gui_res
    endif
endif

# FLTK библиотеки
ifeq ($(IS_WINDOWS), 1)
    # Windows
    GUI_LDLIBS = -lfltk_images -lfltk -lgdiplus -lole32 -luuid -lcomctl32 -lgdi32 -lcomdlg32 -lws2_32 -lwinspool
    GUI_LDFLAGS += -mwindows
else
    # Linux
    GUI_LDLIBS = -lfltk_images -lfltk -lX11 -lXft -lXrender -lfontconfig -lpthread -ldl -lm
endif

# Файл ресурсов (только Windows)
ifneq ($(IS_WINDOWS), 0)
    GUI_RES_FILE = $(GUI_RES_DIR)/icon.rc
    GUI_RES_OBJ = $(GUI_RES_OBJ_DIR)/icon.o
endif

# Поиск исходников проекта (.c)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Исключение main.o для GUI сборки
OBJS_WITHOUT_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

# Поиск исходников GUI (.cpp)
GUI_SRCS = $(wildcard $(GUI_SRC_DIR)/*.cpp)
GUI_OBJS = $(patsubst $(GUI_SRC_DIR)/%.cpp, $(GUI_OBJ_DIR)/%.o, $(GUI_SRCS))

# Поиск файлов тестов (*.cpp), исключая catch_module.cpp
TEST_SRCS = $(filter-out $(TEST_DIR)/catch_module.cpp, $(wildcard $(TEST_DIR)/*.cpp))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# Автоматическая генерация зависимостей (.d)
DEPS = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d) $(GUI_OBJS:.o=.d) $(CATCH_MODULE_OBJ:.o=.d)

# Путь к скомпилированной библиотеке Catch
CATCH_MODULE_OBJ = $(LIB_OBJ_DIR)/catch_module.o

# Финальные пути к исполняемым файлам
EXEC_TARGET = $(EXECUTE_DIR)/$(TARGET)
EXEC_GUI_TARGET = $(EXECUTE_DIR)/$(WIN_TARGET)
EXEC_TEST_TARGET = $(EXECUTE_DIR)/$(TEST_TARGET)

# Главная цель — собирает консольную версию и тесты
all: $(EXEC_TARGET) $(EXEC_TEST_TARGET)
	@echo "The build is complete. For the GUI, use: make winreport"

# Цель для GUI
winreport: $(FLTK_PREFIX)/lib/libfltk.a $(EXEC_GUI_TARGET)
	@echo "GUI winreport build completed"

# Линковка основного исполняемого файла (консольная версия)
$(EXEC_TARGET): $(OBJS) | $(EXECUTE_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC_TARGET) $(LDFLAGS)
	cp $(EXEC_TARGET) ./$(TARGET)

# Линковка GUI приложения
ifeq ($(IS_WINDOWS), 1)
$(EXEC_GUI_TARGET): $(OBJS_WITHOUT_MAIN) $(GUI_OBJS) $(GUI_RES_OBJ) | $(EXECUTE_DIR)
	$(CXX) $(GUI_CXXFLAGS) $(OBJS_WITHOUT_MAIN) $(GUI_OBJS) $(GUI_RES_OBJ) -o $(EXEC_GUI_TARGET) $(GUI_LDFLAGS) $(GUI_LDLIBS)
	cp $(EXEC_GUI_TARGET) ./$(WIN_TARGET)
	@echo "GUI copy app: $(WIN_TARGET)"
else
$(EXEC_GUI_TARGET): $(OBJS_WITHOUT_MAIN) $(GUI_OBJS) | $(EXECUTE_DIR)
	$(CXX) $(GUI_CXXFLAGS) $(OBJS_WITHOUT_MAIN) $(GUI_OBJS) -o $(EXEC_GUI_TARGET) $(GUI_LDFLAGS) $(GUI_LDLIBS)
	cp $(EXEC_GUI_TARGET) ./$(WIN_TARGET)
	@echo "GUI copy app: $(WIN_TARGET)"
endif

# Линковка тестов
$(EXEC_TEST_TARGET): $(CATCH_MODULE_OBJ) $(TEST_OBJS) | $(EXECUTE_DIR)
	$(CXX) $(CXXFLAGS) $(CATCH_MODULE_OBJ) $(TEST_OBJS) -o $(EXEC_TEST_TARGET) $(LDFLAGS)

# Компиляция .c файлов проекта
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция .cpp файлов GUI
$(GUI_OBJ_DIR)/%.o: $(GUI_SRC_DIR)/%.cpp | $(GUI_OBJ_DIR)
	$(CXX) $(GUI_CXXFLAGS) -c $< -o $@

# Компиляция .cpp файлов тестов
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ресурсы консольного приложения (Windows)
ifneq ($(IS_WINDOWS), 0)
$(GUI_RES_OBJ): $(GUI_RES_FILE) | $(GUI_RES_OBJ_DIR)
	$(WINDRES) $< -o $@
endif

# Сборка Catch
$(CATCH_MODULE_OBJ): $(TEST_DIR)/catch_module.cpp $(TEST_HEAD_DIR)/catch.hpp | $(LIB_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(FLTK_PREFIX)/lib/libfltk.a:
	@echo "==> FLTK not found, downloading and compiling..."
	curl -L $(FLTK_URL) -o $(FLTK_ARCHIVE)
	tar -xzf $(FLTK_ARCHIVE)
	cd fltk-$(FLTK_VERSION) && ./configure \
		--prefix=$(FLTK_PREFIX) \
		--disable-shared \
		--enable-gl \
		--enable-threads \
		--disable-debug
	cd fltk-$(FLTK_VERSION) && $(MAKE) -j$(NPROC)
	cd fltk-$(FLTK_VERSION) && $(MAKE) install
	rm -rf fltk-$(FLTK_VERSION) $(FLTK_ARCHIVE)


# Создание директорий
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(GUI_OBJ_DIR):
	mkdir -p $(GUI_OBJ_DIR)

$(LIB_OBJ_DIR):
	mkdir -p $(LIB_OBJ_DIR)

$(EXECUTE_DIR):
	mkdir -p $(EXECUTE_DIR)

ifneq ($(IS_WINDOWS), 0)

$(GUI_RES_OBJ_DIR):
	mkdir -p $(GUI_RES_OBJ_DIR)
endif

# Полная очистка
clean:
	rm -rf $(BUILD_BASE_DIR)
	rm -f ./$(TARGET) ./$(WIN_TARGET)

# Очистка FLTK
clean-fltk:
	rm -rf $(FLTK_PREFIX)


# Подключение сгенерированных файлов зависимостей
-include $(DEPS)

.PHONY: all clean clean-fltk winreport