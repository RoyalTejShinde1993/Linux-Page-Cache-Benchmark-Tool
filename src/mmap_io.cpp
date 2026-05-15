#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include "metrics.hpp"

extern void printResult(const std::string&, double, size_t);

void runMmapBenchmark(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);

    if (fd < 0) {
        perror("open");
        return;
    }

    struct stat st{};
    fstat(fd, &st);

    size_t filesize = st.st_size;

    void* mapped = mmap(nullptr,
                        filesize,
                        PROT_READ,
                        MAP_PRIVATE,
                        fd,
                        0);

    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    volatile char sum = 0;

    Timer timer;

    char* data = static_cast<char*>(mapped);

    for (size_t i = 0; i < filesize; i += 4096) {
        sum += data[i];
    }

    timer.stop();

    munmap(mapped, filesize);
    close(fd);

    printResult("mmap IO", timer.elapsedSeconds(), filesize);
}
