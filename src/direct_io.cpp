#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "metrics.hpp"

extern void printResult(const std::string&, double, size_t);

void runDirectBenchmark(const std::string& path) {
    constexpr size_t BUFFER_SIZE = 4096;

    int fd = open(path.c_str(), O_RDONLY | O_DIRECT);

    if (fd < 0) {
        perror("open");
        return;
    }

    void* buffer;
    if (posix_memalign(&buffer, 4096, BUFFER_SIZE) != 0) {
        std::cerr << "posix_memalign failed\n";
        close(fd);
        return;
    }

    size_t total = 0;

    Timer timer;

    while (true) {
        ssize_t bytes = read(fd, buffer, BUFFER_SIZE);

        if (bytes < 0) {
            perror("read");
            break;
        }

        if (bytes == 0)
            break;

        total += bytes;
    }

    timer.stop();

    free(buffer);
    close(fd);

    printResult("Direct IO", timer.elapsedSeconds(), total);
}
