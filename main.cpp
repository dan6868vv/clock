//Protocol for needle position is number of rotate between [0;360]
#ifdef __unix__
    #include "/home/andrey/raylib/src/raylib.h"
    #include "/home/andrey/raylib/src/raymath.h"
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <stdio.h>

#elif defined(_WIN64)
#include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raylib.h"
#include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raymath.h"
#include <io.h>
#endif
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <string>
#include <sstream>
#define __id "1"
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <fstream>  // Для чтения конфигурационного файла

bool getJsonByPipe(std::unordered_map<std::string, float> &jsonMap, const char *pipe_path, int &fd) {
    if (fd == -1) {
        sleep(10);
        fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
        sleep(10);
        if (errno == EWOULDBLOCK) {
            std::cout << "errno==EWOULDBLOCK" << std::endl;
        }
        perror("open");
        sleep(1);
        return false;
    }

    char buffer[1024];
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    std::string buff;


    if (bytes > 0) {
        buffer[bytes] = '\0';
        buff = std::string(buffer);
    } else {
        perror("read");
        std::cout << "Errno:" << errno << std::endl;
        return false;
    }

    std::stringstream ss(buff);
    std::cout << "Buff: " << buff << std::endl;
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t pos = item.find(':');
        if (pos != std::string::npos) {
            std::string key = item.substr(0, pos);
            if (key == "id" && item.substr(pos + 1) != __id)
                return false;
            try {
                jsonMap[key] = stof(item.substr(pos + 1));
            } catch (const std::invalid_argument &e) {
                std::cout << "❌ Ошибка: неверный формат числа" << std::endl;
                std::cout << "   what(): " << e.what() << std::endl;
            } catch (const std::exception &e) {
                std::cout << "Ошибка: " << e.what() << std::endl;
            }
        }
    }
    return true;
}

bool importModels(std::unordered_map<std::string, float> jsonMap,
                  std::unordered_map<std::string, Model> &modelMap) {
    float id = jsonMap["id"];
    for (auto i: jsonMap) {
        std::cout << i.first << ":" << i.second << std::endl;
        if (i.first == "id")
            continue;
        std::string filePath = "/home/andrey/qwer/clock/_models_for_unix/";

        std::ostringstream oss;
        oss << std::noshowpoint << id; // Убираем десятичную точку, если число целое
        std::string idStr = oss.str(); // "1"
        Model model = LoadModel(
            (filePath + idStr + "/" + i.first + ".obj").c_str());
        modelMap[i.first] = model;
    }
    return true;
}

bool importModels(std::string configLoad,
                  std::unordered_map<std::string, Model> &modelMap) {
    std::stringstream ss(configLoad);
    std::cout << "config[\"load\"]: " << configLoad << std::endl;
    std::string item;
    while (std::getline(ss, item, ',')) {
        std::string filePath = "/home/andrey/qwer/clock/_models_for_unix/";
        std::string idStr = __id;
        Model model = LoadModel(
            (filePath + idStr + "/" + item + ".obj").c_str());
        modelMap[item] = model;
    }
    return true;
}

bool loadModelsByConfig() {
    return true;
}

std::unordered_map<std::string, std::string> loadDataFromConfigFile(const std::string &filename) {
    std::ifstream file(filename);
    std::unordered_map<std::string, std::string> config;
    if (!file.is_open()) {
        std::cout << "Config is not found" << std::endl;
        return config;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string key;
        std::string equals;
        std::string value;
        if (iss >> key >> equals >> value && equals == "=") {
            config[key] = value;
            std::cout << "Данные из config.txt: " << key << " = " << value << std::endl;
        }
    }

    file.close();
    return config;
}

void getDiff(std::unordered_map<std::string, float> jsonMapTarget,
             std::unordered_map<std::string, float> jsonMapCurrent,
             std::unordered_map<std::string, float> &jsonMapDifferent) {
    for (auto itTarget: jsonMapTarget) {
        jsonMapDifferent[itTarget.first] = itTarget.second - jsonMapCurrent[itTarget.first];
    }
}

float convertScaleNumberToAngle(float scale) {
    return -17 * scale / 6 + 360;
}

int main() {
    InitWindow(800, 800, "3D Clock");
    const char *pipe_path = "/tmp/myapp_pipe";
    const std::string id = __id;
    const std::string fileName = "./" + id + "/config.txt";

    std::unordered_map<std::string, float> jsonMapTarget;
    std::unordered_map<std::string, float> jsonMapCurrent;
    std::unordered_map<std::string, float> jsonMapDifferent;
    std::unordered_map<std::string, Model> modelMap;

    std::unordered_map<std::string, std::string> config = loadDataFromConfigFile(fileName);

    mkfifo(pipe_path, 0666);
    int fd = open(pipe_path, O_RDONLY | O_NONBLOCK);

    Camera3D camera = {0};
    camera.position = (Vector3){45.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 2.0f, 0.0f};
    camera.fovy = 90.0f;

    BeginDrawing();
    importModels(config["load"], modelMap);
    for(auto i:modelMap) {
        DrawModel(i.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
    }
    EndDrawing();

    while (!getJsonByPipe(jsonMapTarget, pipe_path, fd)) {
        sleep(10);
    }

//    importModels(jsonMapTarget, modelMap);
    jsonMapCurrent = jsonMapTarget;
    getDiff(jsonMapTarget, jsonMapCurrent, jsonMapDifferent);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        getJsonByPipe(jsonMapTarget, pipe_path, fd);
        getDiff(jsonMapTarget, jsonMapCurrent, jsonMapDifferent);
        for (auto &it: modelMap) {
            jsonMapCurrent[it.first] += 0.4f * jsonMapDifferent[it.first];
            it.second.transform =
                    // MatrixRotateX(DEG2RAD * (jsonMapCurrent[it.first]));
                    MatrixRotateX(DEG2RAD * (convertScaleNumberToAngle(jsonMapCurrent[it.first])));
            DrawModel(it.second, (Vector3){0, 0, 0}, 1.0f, WHITE);
        }

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
        std::cout << "------------------------------------" << std::endl;
        for (auto i: jsonMapTarget) {
            std::cout << "\"" << i.first << "\": \"" << i.second << "\"" << std::endl;
        }
    }

    for (auto it: modelMap) {
        UnloadModel(it.second);
    }
    close(fd);
    CloseWindow();
    return 0;
}
