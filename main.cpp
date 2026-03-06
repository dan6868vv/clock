// #include <cstdio>
//
// // #include "D:\_root\Programs\_Lib_c++\raylib\w64devkit\x86_64-w64-mingw32\include\raylib.h"
// #include "D:\_root\Programs\_Lib_c++\raylib\raylib\src\raylib.h"
// #include "D:\_root\Programs\_Lib_c++\raylib\raylib\src\raymath.h"
//
// int main() {
//     InitWindow(800, 600, "3D Clock");
//
//     // Загружаем модель
//     Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/TV_45.obj");
//
//     // Находим стрелки по индексам (проще всего)
//     // Вам нужно узнать индексы - выведите все названия:
//     for (int i = 0; i < clock.materialCount; i++) {
//
//         printf("Mesh %d\n", i);
//     }
//
//     // Или если знаете названия:
//     // int handIndex = FindModelMeshIndex(clock, "MinuteHand");
//
//     Camera3D camera = { 0 };
//     camera.position = (Vector3){ 50.0f, 50.0f, 100.0f };
//     camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
//     camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
//     camera.fovy = 45.0f;
//
//     float rotation = 0.0f;
//
//     while (!WindowShouldClose()) {
//         rotation += 0.1f; // вращение
//
//         // Вращаем стрелку (предполагаем что это mesh с индексом 1)
//
//         clock.transform = MatrixRotateX(DEG2RAD * rotation);
//
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//
//         BeginMode3D(camera);
//         DrawModel(clock, (Vector3){0,0,0}, 1.0f, WHITE);
//         DrawGrid(10, 1.0f);
//         EndMode3D();
//
//         DrawFPS(10, 10);
//         EndDrawing();
//     }
//
//     UnloadModel(clock);
//     CloseWindow();
//     return 0;
// }


#include <cstdio>
// #include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raylib.h"
// #include "D:/_root/Programs/_Lib_c++/raylib/raylib/src/raymath.h"

#include "/home/andrey/raylib/src/raylib.h"
#include "/home/andrey/raylib/src/raymath.h"

int main() {
   InitWindow(800, 800, "3D Clock");

    // Флаг для отслеживания полноэкранного режима
    bool isFullscreen = true;
  //  ToggleFullscreen(); // Включаем полноэкранный режим

    // Загружаем модель
    // Model clock = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/TV_46.obj");
    // Model needle = LoadModel("D:/_root/Job/AeroMash_new/Arrow_Display/tv_45/strelka.obj");

    Model clock = LoadModel("/home/andrey/qwer/clock/TV_46.obj");
    Model needle = LoadModel("/home/andrey/qwer/clock/tv_45/strelka.obj");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 45.0f, 0.0f, 0.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.fovy = 90.0f;

    float rotation = 0.0f;

    while (!WindowShouldClose()) {
        // Проверяем нажатие ESC для выхода из полноэкранного режима
        if (IsKeyPressed(KEY_SPACE) && isFullscreen) {
            ToggleFullscreen(); // Выходим из полноэкранного режима
            SetWindowSize(500,500);
            isFullscreen = false;
        }
       // RestoreWindow();
        // Альтернативный вариант: повторное нажатие ESC для переключения туда-обратно
        // if (IsKeyPressed(KEY_ESCAPE)) {
        //     ToggleFullscreen();
        //     isFullscreen = !isFullscreen;
        // }

        rotation -= 0.1f; // скорость вращения

        // ВАЖНО: применяем поворот ко ВСЕЙ модели сразу
        needle.transform = MatrixRotateX(DEG2RAD * rotation); // вращение вокруг X

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Рисуем всю модель целиком
        DrawModel(needle, (Vector3){0, 0, 0}, 1.0f, WHITE);
        DrawModel(clock, (Vector3){0, 0, 0}, 1.0f, WHITE);

        DrawGrid(10, 1.0f);
        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadModel(clock);
    CloseWindow();
    return 0;
}