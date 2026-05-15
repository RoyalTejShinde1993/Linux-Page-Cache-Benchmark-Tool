#include "metrics.hpp"
#include <algorithm>
#include <iostream>
#include <string>

#if defined(__unix__) || defined(__APPLE__)
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

extern void printResult(const std::string&, double, size_t);

void runMmapBenchmark(const std::string& path,
                      std::size_t blockSize,
                      std::size_t maxBytes) {
#if defined(__unix__) || defined(__APPLE__)
    if (blockSize == 0) {
        std::cerr << "mmap block size must be non-zero. Using 4096.\n";
        blockSize = 4096;
    }

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    struct stat st{};
    if (fstat(fd, &st) != 0) {
        perror("fstat");
        close(fd);
        return;
    }

    std::size_t filesize = static_cast<std::size_t>(st.st_size);
    std::size_t length = filesize;
    if (maxBytes != SIZE_MAX) {
        length = std::min(filesize, maxBytes);
    }

    if (length == 0) {
        std::cerr << "No bytes mapped for mmap benchmark: " << path << "\n";
        close(fd);
        return;
    }

    void* mapped = mmap(nullptr,
                        length,
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
    for (std::size_t offset = 0; offset < length; offset += blockSize) {
        sum += data[offset];
    }
    timer.stop();

    munmap(mapped, length);
    close(fd);
    printResult("mmap IO", timer.elapsedSeconds(), length);
#else
    std::cerr << "mmap benchmark is only supported on Unix-like systems.\n";
#endif
}
