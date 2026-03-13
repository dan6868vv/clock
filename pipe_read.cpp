#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>

int main() {
    const char *pipe_path = "/tmp/myapp_pipe";

    // Создаем канал
    mkfifo(pipe_path, 0666);

    std::cout << "Читатель запущен. Ожидание данных..." << std::endl;
    int fd = open(pipe_path, O_RDONLY | O_NONBLOCK);

    while (true) {

        if (fd == -1) {
            if(errno==EWOULDBLOCK) {
                std::cout << "errno==EWOULDBLOCK" << std::endl;
                sleep(10);
               // continue;
            }
            perror("open");
            sleep(10);
            fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
            continue;
        }

        char buffer[1024];
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "Получено: ";
            // for(int i=0;i<bytes;i++) {
            //     std::cout << buffer[i];
            // }
            std::cout << std::endl;
            std::cout << "Получено: " << std::string(buffer) << std::endl;
            // std::cout << "Получено: " << atof(buffer) << std::endl;
            //         std::cout << "Size: " << sizeof(atof(buffer)) << std::endl;
        }

    }
    close(fd);

    return 0;
}