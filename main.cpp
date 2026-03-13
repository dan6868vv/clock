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
#include <chrono>
#ifdef __unix__
float getAngleByPipe() {
    const char *pipe_path = "/tmp/myapp_pipe";
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

bool getJsonByPipe(std::unordered_map<std::string, float> &jsonMap) {
    const char *pipe_path = "/tmp/myapp_pipe";
    mkfifo(pipe_path, 0666);

    int fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
    // int fd = open(pipe_path, O_RDONLY);
    if (fd == -1) {
        if(errno==EWOULDBLOCK) {
            std::cout << "errno==EWOULDBLOCK" << std::endl;
            sleep(10);
        }
        perror("open");
        sleep(1);
        close(fd);
        return false;
    }

    char buffer[1024];
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    std::string buff;


    if (bytes > 0) {
        buffer[bytes] = '\0';
        buff = std::string(buffer);
    }
    else {
        close(fd);
        return false;
    }
    close(fd);
    std::cout << "Buffer:" << std::endl;
    std::cout << buff << std::endl;

    std::stringstream ss(buff);
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t pos = item.find(':');
        if (pos != std::string::npos) {
            std::string key = item.substr(0, pos);
            if (key == "id" && item.substr(pos + 1) != __id)
                return false;
            jsonMap[key] = stof(item.substr(pos + 1));
        }
    }
    return true;
}
#endif

bool importModels(std::unordered_map<std::string, float> jsonMap,
                  std::unordered_map<std::string, Model> &modelMap) {
    float id = jsonMap["id"];
    for (auto i: jsonMap) {
        std::cout << i.first << ":" << i.second << std::endl;
        if (i.first == "id")
            continue;
#ifdef __unix__
        std::string filePath = "/home/andrey/qwer/clock/_models_for_unix/";
#elif defined(_WIN64)
        std::string filePath = "D:/_root/Job/AeroMash_new/Arrow_Display/_models_for_win/";
#endif
        std::ostringstream oss;
        oss << std::noshowpoint << id; // Убираем десятичную точку, если число целое
        std::string idStr = oss.str(); // "1"
        Model model = LoadModel(
            (filePath + idStr + "/" + i.first + ".obj").c_str());
        modelMap[i.first] = model;
    }
    return true;
}

void getDiff(std::unordered_map<std::string, float> jsonMapTarget,
             std::unordered_map<std::string, float> jsonMapCurrent,
             std::unordered_map<std::string, float> &jsonMapDifferent) {
    for (auto itTarget: jsonMapTarget) {
        jsonMapDifferent[itTarget.first] = itTarget.second - jsonMapCurrent[itTarget.first];
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
    std::string line;

#ifdef __unix__
    while(!getJsonByPipe(jsonMapTarget)) {
        sleep(10);
    }
        importModels(jsonMapTarget, modelMap);
        jsonMapCurrent = jsonMapTarget;
        getDiff(jsonMapTarget,jsonMapCurrent, jsonMapDifferent);

#elif defined(_WIN64)
    float angle = 0;
    angle += 1;
#endif

    std::string lastLine;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
#ifdef __unix__
        getJsonByPipe(jsonMapTarget);

        getDiff(jsonMapTarget, jsonMapCurrent, jsonMapDifferent);
        for (auto &it: modelMap) {
            jsonMapCurrent[it.first] += 0.5f * jsonMapDifferent[it.first];
            it.second.transform =
                    MatrixRotateX(DEG2RAD * (jsonMapCurrent[it.first]));
         //   std::cout << "Draw " << it.first << " diffe: " << jsonMapDifferent[it.first] << std::endl;
         //   std::cout << "Draw " << it.first << " angle: " << jsonMapCurrent[it.first] << std::endl;
            DrawModel(it.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
        }
#endif

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
        std::cout << "In for" << std::endl;
        for (auto i: jsonMapTarget) {
            std::cout << i.first << ": " << i.second << std::endl;
        }
        std::cout << "After for" << std::endl;
    }

    for (auto it: modelMap) {
        UnloadModel(it.second);
    }

    CloseWindow();
    return 0;
}
