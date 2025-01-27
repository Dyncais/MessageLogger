CXX = g++
CXXFLAGS = -std=c++17 -fPIC -Wall -Wextra
LDFLAGS = -Lbuild -llogger
RPATH = -Wl,-rpath,$(shell pwd)/build

# Папка для сборки
BUILD_DIR = build

# Названия целей
LIB_TARGET = $(BUILD_DIR)/liblogger.so
APP_TARGET = $(BUILD_DIR)/app
TEST_TARGET = $(BUILD_DIR)/test_logger

LIB_SRC = logger.cpp
APP_SRC = main.cpp
TEST_SRC = test_logger.cpp

all: $(APP_TARGET)

# Создание папки сборки, если её нет
$(BUILD_DIR):
	@mkdir -p $@

# Сборка динамической библиотеки
liblogger.so: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_SRC) | $(BUILD_DIR)
	@echo "Building library: $(LIB_TARGET)"
	$(CXX) $(CXXFLAGS) -shared -o $@ $(LIB_SRC)

# Сборка приложения
app: $(APP_TARGET)

$(APP_TARGET): $(APP_SRC) $(LIB_TARGET) | $(BUILD_DIR)
	@echo "Building application: $(APP_TARGET)"
	$(CXX) $(CXXFLAGS) -o $@ $(APP_SRC) $(LDFLAGS) $(RPATH)

# Сборка тестов
test_logger: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(LIB_TARGET) | $(BUILD_DIR)
	@echo "Building tests: $(TEST_TARGET)"
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) $(LDFLAGS) $(RPATH)

# Запуск приложения
run: $(APP_TARGET)
	@echo "Running application: $(APP_TARGET)"
	LD_LIBRARY_PATH=$(BUILD_DIR) ./$(APP_TARGET)

# Запуск тестов
test: $(TEST_TARGET)
	@echo "Running tests: $(TEST_TARGET)"
	LD_LIBRARY_PATH=$(BUILD_DIR) ./$(TEST_TARGET)

# Очистка
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
