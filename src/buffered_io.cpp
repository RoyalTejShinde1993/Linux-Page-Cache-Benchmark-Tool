#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "metrics.hpp"

extern void printResult(const std::string&, double, size_t);

void runBufferedBenchmark(const std::string& path,
                          std::size_t blockSize,
                          std::size_t maxBytes) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for buffered IO: " << path << "\n";
        return;
    }

    std::vector<char> buffer(blockSize);
    std::size_t total = 0;

    Timer timer;
    while (total < maxBytes && (file.read(buffer.data(), static_cast<std::streamsize>(std::min(blockSize, maxBytes - total))) || file.gcount() > 0)) {
        total += static_cast<std::size_t>(file.gcount());
    }
    timer.stop();

    printResult("Buffered IO", timer.elapsedSeconds(), total);
}
