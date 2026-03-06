# Имя исполняемого файла
TARGET = clock_program

# Компилятор
CXX = g++

# Пути к библиотеке raylib
RAYLIB_PATH = $(HOME)/raylib/build
INCLUDES = -I$(RAYLIB_PATH)/raylib/include
LIBS_PATH = -L$(RAYLIB_PATH)/raylib

# Библиотеки для линковки
LIBS = -lraylib -lGLESv2 -lEGL -lpthread -lm -ldl -lrt

# Флаги компиляции
CXXFLAGS = -Wall -std=c++11

# Исходные файлы
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Правило по умолчанию
all: $(TARGET)

# Линковка
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS_PATH) $(LIBS)

# Компиляция
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Установка (если нужно скопировать куда-то)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

.PHONY: all clean run install