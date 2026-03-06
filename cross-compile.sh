#!/bin/bash

# Очищаем предыдущую сборку
rm -rf build
mkdir build
cd build

# Настраиваем компиляцию
cmake .. \
    -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
    -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
    -DBUILD_SHARED_LIBS=ON \
    -DPLATFORM="Desktop" \
    -DUSE_WAYLAND=OFF \
    -DUSE_X11=OFF \
    -DOPENGL_VERSION=ES2 \
    -DCMAKE_FIND_ROOT_PATH=/usr/arm-linux-gnueabihf \
    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY

# Собираем
make -j4

echo "✅ Сборка raylib завершена!"
