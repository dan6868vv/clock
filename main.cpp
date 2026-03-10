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

void getJsonByPipe(std::unordered_map<std::string, float> &jsonMap) {
    const char* pipe_path = "/tmp/myapp_pipe";
    mkfifo(pipe_path, 0666);
    bool flag = true;
    while (flag) {
        int fd = open(pipe_path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            break;
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
                if(key == "id" && item.substr(pos + 1)!=__id)
                    return;
                jsonMap[key] = stof(item.substr(pos + 1));
            }
        }
        break;
    }
}
#endif

bool importModels(std::unordered_map<std::string, float> jsonMap,
                  std::unordered_map<std::string, Model> &modelMap) {
    float id = jsonMap["id"];
    for (auto i: jsonMap) {
        if (i.first == "id")
            continue;
#ifdef __unix__
        std::string filePath = "/home/andrey/qwer/clock/_models_for_unix/";
#elif defined(_WIN64)
        std::string filePath = "D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/";
#endif
        Model model = LoadModel(
            (filePath + std::to_string(id) + "/" + i.first + ".obj").c_str());
        modelMap[i.first] = model;
    }
    return true;
}

void getDiff(std::unordered_map<std::string, std::string> jsonMapTarget,
             std::unordered_map<std::string, std::string> jsonMapCurrent,
             std::unordered_map<std::string, float> &jsonMapDifferent) {
    for (auto itTarget: jsonMapTarget) {
        jsonMapDifferent[itTarget.first] = std::stof(itTarget.second) - std::stof(jsonMapCurrent[itTarget.first]);
    }
}

int main() {
    InitWindow(800, 800, "3D Clock");

#ifdef _WIN64
    Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_frame.obj");
    Model needle = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/tv-45_needle.obj");
#endif

    Camera3D camera = {0};
    camera.position = (Vector3){45.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 2.0f, 0.0f};
    camera.fovy = 90.0f;

    std::unordered_map<std::string, float> jsonMapTarget;
    std::unordered_map<std::string, float> jsonMapCurrent;
    std::unordered_map<std::string, float> jsonMapDifferent;
    std::unordered_map<std::string, Model> modelMap;
#ifdef __unix__
    getJsonByPipe(jsonMapTarget);
    importModels(jsonMapTarget, modelMap);
    jsonMapCurrent = jsonMapTarget;
#elif defined(_WIN64)
    float angle = 0;
    angle += 1;
#endif
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
    while (!WindowShouldClose()) {
#ifdef __unix__
        getJsonByPipe(jsonMapTarget);
        for (auto &it: modelMap) {
            it.second.transform =
                    MatrixRotateX(DEG2RAD * jsonMapCurrent[it.first]);
            DrawModel(it.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
        }
#endif

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();

        for (auto i: jsonMapTarget) {
            std::cout << i.first << ": " << i.second << std::endl;
        }
    }

    for (auto it: modelMap) {
        UnloadModel(it.second);
    }

    CloseWindow();
    return 0;
}
