#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "metrics.hpp"

extern void printResult(const std::string&, double, size_t);

void runBufferedBenchmark(const std::string& path) {
    constexpr size_t BUFFER_SIZE = 4096;

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    std::vector<char> buffer(BUFFER_SIZE);

    size_t total = 0;

    Timer timer;

    while (true) {
        ssize_t bytes = read(fd, buffer.data(), BUFFER_SIZE);

        if (bytes < 0) {
            perror("read");
            break;
        }

        if (bytes == 0)
            break;

        total += bytes;
    }

    timer.stop();

    close(fd);

    printResult("Buffered IO", timer.elapsedSeconds(), total);
}
