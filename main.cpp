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
#define __id "1"

// #ifdef __unix__
// float getAngleByPipe() {
//     const char* pipe_path = "/tmp/myapp_pipe";
//     // Создаем канал
//     mkfifo(pipe_path, 0666);
//    // std::cout << "Читатель запущен. Ожидание данных..." << std::endl;
//     bool flag = true;
//     while (flag) {
//         int fd = open(pipe_path, O_RDONLY);
//         if (fd == -1) {
//             perror("open");
//             sleep(1);
//             continue;
//         }
//         char buffer[1024];
//         ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
//         if (bytes > 0) {
//             buffer[bytes] = '\0';
//        //     std::cout << "Получено: " << buffer << std::endl;
//         }
//         close(fd);
//         return atof(buffer);
//     }
//     return 0;
// }
//
// void getJsonByPipe(std::unordered_map<std::string, std::string> &jsonMap) {
//     const char* pipe_path = "/tmp/myapp_pipe";
//     mkfifo(pipe_path, 0666);
//     bool flag = true;
//     while (flag) {
//         int fd = open(pipe_path, O_RDONLY);
//         if (fd == -1) {
//             perror("open");
//             break;
//         }
//         char buffer[1024];
//         ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
//         if (bytes > 0) {
//             buffer[bytes] = '\0';
//         }
//         close(fd);
//
//         std::string buff = std::string(buffer);
//
//         std::stringstream ss(buff);
//         std::string item;
//         while (std::getline(ss, item, ',')) {
//             size_t pos = item.find(':');
//             if (pos != std::string::npos) {
//                 std::string key = item.substr(0, pos);
//                 if(key == "id" && item.substr(pos + 1)!=__id)
//                     return;
//                 jsonMap[key] = item.substr(pos + 1);
//             }
//         }
//         break;
//     }
// }
// #endif
//
// bool importModels(std::unordered_map<std::string, std::string> jsonMap,
//                   std::unordered_map<std::string, Model> &modelMap) {
//     std::string id = jsonMap["id"];
//     for (auto i: jsonMap) {
//         if (i.first == "id")
//             continue;
// #ifdef __unix__
//         Model model = LoadModel(("/home/andrey/qwer/clock/_models_for_unix/" + id +"/" + i.first+ ".obj").c_str());
//         modelMap[i.first] = model;
// #elif defined(_WIN64)
//         Model model = LoadModel(("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/" + id +"/" + i.first + ".obj").c_str());
//         modelMap[i.first] = model;
// #endif
//     }
//     return true;
// }
//
// int main() {
//     InitWindow(800, 800, "3D Clock");
//
// #ifdef _WIN64
//     Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_frame.obj");
//     Model needle = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_needle.obj");
// #endif
//
//     Camera3D camera = {0};
//     camera.position = (Vector3){45.0f, 0.0f, 0.0f};
//     camera.target = (Vector3){0.0f, 0.0f, 0.0f};
//     camera.up = (Vector3){0.0f, 2.0f, 0.0f};
//     camera.fovy = 90.0f;
//
//     std::unordered_map<std::string, std::string> jsonMap;
//     std::unordered_map<std::string, Model> modelMap;
// #ifdef __unix__
//     getJsonByPipe(jsonMap);
//     importModels(jsonMap,modelMap);
// #elif defined(_WIN64)
//     float angle = 0;
//     angle += 1;
// #endif
//     BeginDrawing();
//     ClearBackground(RAYWHITE);
//     BeginMode3D(camera);
//     while (!WindowShouldClose()) {
//
// #ifdef __unix__
//         getJsonByPipe(jsonMap);
//         for(auto& it:modelMap) {
//             it.second.transform = MatrixRotateX(DEG2RAD * stof(jsonMap[it.first]));
//             DrawModel(it.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
//         }
// #endif
//
//         EndMode3D();
//         DrawFPS(10, 10);
//         EndDrawing();
//
//         for (auto i: jsonMap) {
//             std::cout << i.first << ": " << i.second << std::endl;
//         }
//     }
//
//     for(auto it:modelMap) {
//         UnloadModel(it.second);
//     }
//
//     CloseWindow();
//     return 0;
// }
int main() {
    InitWindow(800, 800, "3D Clock");

    Camera3D camera = {0};
    camera.position = (Vector3){45.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 2.0f, 0.0f};
    camera.fovy = 90.0f;

    std::unordered_map<std::string, std::string> jsonMap;
    std::unordered_map<std::string, Model> modelMap;
    std::unordered_map<std::string, float> currentAngles; // Текущие углы
    std::unordered_map<std::string, float> targetAngles;  // Целевые углы

    float smoothFactor = 0.15f; // Коэффициент плавности (0-1, где 1 - мгновенно)

#ifdef __unix__
    getJsonByPipe(jsonMap);
    importModels(jsonMap, modelMap);

    // Инициализируем углы
    for(auto& it : modelMap) {
        currentAngles[it.first] = 0.0f;
        if (jsonMap.find(it.first) != jsonMap.end()) {
            targetAngles[it.first] = std::stof(jsonMap[it.first]);
        } else {
            targetAngles[it.first] = 0.0f;
        }
    }
#endif

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

#ifdef __unix__
        // Получаем новые данные
        getJsonByPipe(jsonMap);

        // Обновляем целевые углы
        for(auto& it : modelMap) {
            if (jsonMap.find(it.first) != jsonMap.end()) {
                targetAngles[it.first] = std::stof(jsonMap[it.first]);
            }
        }

        // Плавно поворачиваем каждую модель
        for(auto& it : modelMap) {
            float& current = currentAngles[it.first];
            float target = targetAngles[it.first];

            // Находим кратчайший путь
            float diff = target - current;
            if (diff > 180.0f) diff -= 360.0f;
            if (diff < -180.0f) diff += 360.0f;

            // Применяем плавность
            current += diff * smoothFactor * (deltaTime * 60.0f); // Нормализуем по времени

            // Нормализуем угол
            if (current >= 360.0f) current -= 360.0f;
            if (current < 0.0f) current += 360.0f;

            // Применяем поворот
            it.second.transform = MatrixRotateX(DEG2RAD * current);
            DrawModel(it.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
        }
#endif

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup...
    return 0;
}