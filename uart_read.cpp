//Thid code read uart and push it to pipe

#include <iostream>
#include <string>
#include <cmath>
#include <errno.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FLOAT_TO_INT(x) ((x)>=0.0f?(int)((x)+0.5f):(int)((x)-0.5f))


// Класс для накопления строки из UART
class UARTLineReader {
private:
    std::string currentLine;
    int fd;

public:
    UARTLineReader(int serialFd) : fd(serialFd) {
    }

    // Проверяет, есть ли полная строка (до \n)
    bool readLine(std::string &line) {
        while (serialDataAvail(fd) > 0) {
            char received = serialGetchar(fd);

            if (received == '\n') {
                line = currentLine;
                currentLine.clear();
                return true;
            } else if (received != '\r') {
                currentLine += received;
            }
        }
        return false;
    }

    // Читает и пытается преобразовать в число
    bool readFloat(float &value) {
        std::string line;
        if (readLine(line)) {
            try {
                value = std::stof(line);
                // Нормализуем угол в диапазон 0-360
                value = fmod(value, 360.0f);
                if (value < 0) value += 360.0f;
                return true;
            } catch (const std::exception &e) {
                std::cout << "Получено не число: " << line << std::endl;
            }
        }
        return false;
    }

    bool readJSON(std::string &json) {
        //    std::string line;
        if (readLine(json)) {
            return true;
        }
        return false;
    }
};


void readUartFloatPushItToPipe(UARTLineReader *reader, std::string pipe_path) {
    float angle = 0;
    if (reader->readFloat(angle)) {
        std::cout << angle << std::endl;
        int fd2 = open(pipe_path.c_str(), O_WRONLY | O_NONBLOCK);
        if (fd2 == -1) {
            std::cerr << "Ошибка открытия канала FIFO Pipe" << std::endl;
            perror("open");
        }
        std::string angleByString = std::to_string(angle);
        if ((write(fd2, angleByString.c_str(), angleByString.length())) == -1) {
            perror("write");
        }
        close(fd2);
    }
}

// void readUsrtStringPushItToPipe(UARTLineReader *reader, std::string pipe_path) {
//     std::string json = "";
//     if (reader->readJSON(json)) {
//         std::cout << json << std::endl;
//         int fd2 = open(pipe_path.c_str(), O_WRONLY | O_NONBLOCK);
//         if (fd2 == -1) {
//             std::cerr << "Ошибка открытия канала FIFO Pipe" << std::endl;
//             perror("open");
//         } else ((write(fd2, json.c_str(), json.length())) == -1) {
//             perror("write");
//         }
//         // std::cout << "Strint to push: "<< angleByString << std::endl;
//
//         close(fd2);
//     }
// }
void readUsrtStringPushItToPipe(std::string json, std::string pipe_path) {
    // Пытаемся открыть канал
    // int fd2 = open(pipe_path.c_str(), O_WRONLY | O_NONBLOCK);
    int fd2 = open(pipe_path.c_str(), O_WRONLY );
    if (fd2 == -1) {
        // Анализируем причину ошибки
        switch (errno) {
            case ENXIO:
                // Это нормально - просто нет читателя
                std::cout << "📭 Нет читателя канала (данные не отправлены)" << std::endl;
                break;
            case ENOENT:
                std::cerr << "❌ Канал не существует" << std::endl;
                break;
            case EACCES:
                std::cerr << "❌ Нет прав доступа к каналу" << std::endl;
                break;
            default:
                std::cerr << "❌ Ошибка открытия: " << strerror(errno) << std::endl;
        }
        return;
    }

    // Канал открыт успешно
    std::cout << "🔗 Канал открыт, fd=" << fd2 << std::endl;

    // Пытаемся записать
    std::cout << "Tru to write str: " << json << std::endl;
    // ssize_t written = write(fd2, json.c_str(), json.length());
    ssize_t written;
    try {
        written = write(fd2, json.c_str(), json.length());

        if (written == -1) {
            // Обработка ошибок write
            throw std::system_error(errno, std::generic_category(), "Ошибка записи в pipe");
        }

        std::cout << "✅ Записано " << written << " байт" << std::endl;

    } catch (const std::system_error& e) {
        std::cerr << "❌ Системная ошибка: " << e.what() << std::endl;
        if (e.code().value() == EPIPE) {
            std::cout << "📪 Читатель закрыл канал" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "❌ Неизвестная ошибка!" << std::endl;
    }

    std::cout << "Written: " << written << std::endl;
    std::cout << "Errno: " << errno << std::endl;
    if (written == -1) {
        std::cout << "Errno: " << errno << std::endl;
        if (errno == EPIPE) {
            std::cout << "📪 Читатель закрыл канал во время записи" << std::endl;
        } else if (errno == EWOULDBLOCK) {
            std::cout << "errno == EWOULDBLOCK" << std::endl;
            sleep(5);
            return;
        } else {
            std::cerr << "❌ Ошибка записи: " << strerror(errno) << std::endl;
        }
    } else {
        std::cout << "✅ Записано " << written << " байт в канал" << std::endl;
    }
    close(fd2);

    // Всегда закрываем дескриптор
    std::cout << "🔒 Канал закрыт" << std::endl;
}

int main(int argc, char **argv) {
    // Параметры по умолчанию
    std::string port = "/dev/serial0";
    int baudRate = 9600;

    // Можно передать порт и скорость как аргументы командной строки
    if (argc > 1) {
        port = argv[1];
    }
    if (argc > 2) {
        baudRate = std::stoi(argv[2]);
    }
    std::cout << "======================================" << std::endl;
    std::cout << "Порт: " << port << std::endl;
    std::cout << "Скорость: " << baudRate << std::endl;

    // Инициализация wiringPi
    if (wiringPiSetup() == -1) {
        std::cerr << "❌ Ошибка инициализации wiringPi" << std::endl;
        return 1;
    }

    // Открываем последовательный порт
    int fd = serialOpen(port.c_str(), baudRate);
    if (fd == -1) {
        std::cerr << "❌ Не удалось открыть порт: " << strerror(errno) << std::endl;
        std::cerr << "⚠️ Продолжаем в демо-режиме (без UART)..." << std::endl;
    } else {
        std::cout << "✅ Порт открыт успешно. Ожидание данных..." << std::endl;
    }

    // Создаем читатель UART
    UARTLineReader *reader = nullptr;
    reader = new UARTLineReader(fd);
    std::string pipe_path = "/tmp/myapp_pipe";
    if (mkfifo(pipe_path.c_str(), 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    while (true) {
        if (fd != -1 && reader != nullptr) {
            //    readUartFloatPushItToPipe(reader, pipe_path);
            std::string json = "";
            if (!reader->readJSON(json)) {
                continue;
            }
            std::cout << "json: " << json << std::endl;
            readUsrtStringPushItToPipe(json, pipe_path);
        } else {
            std::cout << "\033[31mUART не определен\033[0m" << std::endl;
        }
        //   sleep(5);
    }

    // Очистка
    if (reader != nullptr) {
        delete reader;
    }

    if (fd != -1) {
        serialClose(fd);
        std::cout << "Порт закрыт" << std::endl;
    }
    return 0;
}
