#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    const char *pipe_path = "/tmp/myapp_pipe";

    // Создаем канал
    mkfifo(pipe_path, 0666);

    std::cout << "Читатель запущен. Ожидание данных..." << std::endl;

    while (true) {
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
            std::cout << "Получено: " << buffer << std::endl;
            // std::cout << "Получено: " << atof(buffer) << std::endl;
   //         std::cout << "Size: " << sizeof(atof(buffer)) << std::endl;
        }

        close(fd);
    }

    return 0;
}
