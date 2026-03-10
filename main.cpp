//Protocol for needle position is number of rotate between [0;360]
#ifdef __unix__
    #include "/home/andrey/raylib/src/raylib.h"
    #include "/home/andrey/raylib/src/raymath.h"
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <stdio.h>

#elif defined(_WIN64)
#include <cstdio>
#include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raylib.h"
#include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raymath.h"
#endif
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#ifdef __unix__
float getAngleByPipe() {
    const char* pipe_path = "/tmp/myapp_pipe";
    // Создаем канал
    mkfifo(pipe_path, 0666);
   // std::cout << "Читатель запущен. Ожидание данных..." << std::endl;
    bool flag = true;
    while (flag) {
        int fd = open(pipe_path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            sleep(1);
            continue;
        }
        char buffer[1024];
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
       //     std::cout << "Получено: " << buffer << std::endl;
        }
        close(fd);
        return atof(buffer);
    }
    return 0;
}

void getJsonByPipe(std::unordered_map<std::string, std::string> &jsonMap) {
    const char* pipe_path = "/tmp/myapp_pipe";
    mkfifo(pipe_path, 0666);
    bool flag = true;
    while (flag) {
        int fd = open(pipe_path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            sleep(1);
            continue;
        }
        char buffer[1024];
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
        }
        close(fd);

        std::string buff = std::string(buffer);

        std::stringstream ss(buff);
        std::string item;
        while (std::getline(ss, item, ',')) {
            size_t pos = item.find(':');
            if (pos != std::string::npos) {
                std::string key = item.substr(0, pos);
               // int value = std::stoi(item.substr(pos + 1));
                jsonMap[key] = item.substr(pos + 1);

            }
        }
        for(auto i:jsonMap) {
            std::cout << i.first << ": " << i.second << std::endl;
        }
    }
   // return 0;
}
#endif
int main() {
    InitWindow(800, 800, "3D Clock");

    // Флаг для отслеживания полноэкранного режима
    //  bool isFullscreen = true;
    //  ToggleFullscreen(); // Включаем полноэкранный режим

#ifdef __unix__
    // Model clock = LoadModel("/home/andrey/qwer/clock/_models_for_unix/tv-45_frame.obj");
 //    Model needle = LoadModel("/home/andrey/qwer/clock/_models_for_unix/tv-45_needle.obj");
    Model clock = LoadModel("/home/andrey/qwer/clock/_models_for_unix/ite_2t_2/scale.obj");
    Model needle = LoadModel("/home/andrey/qwer/clock/_models_for_unix/ite_2t_2/needle_2.obj");
 //   Model yellow = LoadModel("/home/andrey/qwer/clock/_models_for_unix/ite_2t_2/yellow.obj");
#elif defined(_WIN64)
    // Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_frame.obj");
    // Model needle = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_needle.obj");
    Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_frame.obj");
    Model needle = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_needle.obj");
#endif

    Camera3D camera = {0};
    camera.position = (Vector3){45.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 2.0f, 0.0f};
    camera.fovy = 90.0f;

    float rotation = 0.0f;
    float angle = 0;

    std::unordered_map<std::string, std::string> jsonMap;

    while (!WindowShouldClose()) {
        // Проверяем нажатие ESC для выхода из полноэкранного режима
        // if (IsKeyPressed(KEY_SPACE) && isFullscreen) {
        //     ToggleFullscreen(); // Выходим из полноэкранного режима
        //     SetWindowSize(500,500);
        //     isFullscreen = false;
        // }
        // RestoreWindow();
        // Альтернативный вариант: повторное нажатие ESC для переключения туда-обратно
        // if (IsKeyPressed(KEY_ESCAPE)) {
        //     ToggleFullscreen();
        //     isFullscreen = !isFullscreen;
        // }
        rotation -= 0.1f; // скорость вращения


        // ВАЖНО: применяем поворот ко ВСЕЙ модели сразу
        //    needle.transform = MatrixRotateX(DEG2RAD * rotation); // вращение вокруг X
        needle.transform = MatrixRotateX(DEG2RAD * angle); // вращение вокруг X

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Рисуем всю модель целиком
        DrawModel(needle, (Vector3){0, 0, 0}, 1.0f, WHITE);
        DrawModel(clock, (Vector3){0, 0, 0}, 1.0f, WHITE);
        //     DrawModel(yellow, (Vector3){0, 0, 0}, 1.0f, WHITE);

        DrawGrid(10, 1.0f);
        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
#ifdef __unix__
//        angle = getAngleByPipe();
        getJsonByPipe(jsonMap);
#elif defined(_WIN64)
        angle += 1;
#endif

        for(auto i:jsonMap) {
            std::cout << i.first << ": " << i.second << std::endl;
        }
    }

    UnloadModel(clock);
    UnloadModel(needle);
    //  UnloadModel(needle);
    CloseWindow();
    return 0;
}
