#include "metrics.hpp"
#include <iostream>
#include <string>

#if defined(__unix__) || defined(__APPLE__)
#  include <fcntl.h>
#  include <unistd.h>
#  include <cstdlib>
#endif

extern void printResult(const std::string&, double, size_t);

void runDirectBenchmark(const std::string& path,
                        std::size_t blockSize,
                        std::size_t maxBytes) {
#if defined(__linux__) && defined(O_DIRECT)
    if (blockSize == 0 || blockSize % 4096 != 0) {
        std::cerr << "Direct IO block size must be a non-zero multiple of 4096. Using 4096.\n";
        blockSize = 4096;
    }

    int fd = open(path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open");
        return;
    }

    void* buffer = nullptr;
    if (posix_memalign(&buffer, 4096, blockSize) != 0) {
        std::cerr << "posix_memalign failed\n";
        close(fd);
        return;
    }

    std::size_t total = 0;
    Timer timer;
    while (total < maxBytes) {
        std::size_t chunk = std::min(blockSize, maxBytes - total);
        ssize_t bytes = read(fd, buffer, chunk);
        if (bytes < 0) {
            perror("read");
            break;
        }
        if (bytes == 0)
            break;
        total += static_cast<std::size_t>(bytes);
    }
    timer.stop();

    free(buffer);
    close(fd);
    printResult("Direct IO", timer.elapsedSeconds(), total);
#else
    std::cerr << "Direct IO benchmark is only supported on Linux with O_DIRECT.\n";
#endif
}
