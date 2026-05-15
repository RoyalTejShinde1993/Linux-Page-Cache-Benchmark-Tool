#include <iostream>
#include "benchmark.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./pagebench <buffered|direct|mmap> <file>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string file = argv[2];

    if (mode == "buffered") {
        runBufferedBenchmark(file);
    } else if (mode == "direct") {
        runDirectBenchmark(file);
    } else if (mode == "mmap") {
        runMmapBenchmark(file);
    } else {
        std::cout << "Unknown mode\n";
        return 1;
    }

    return 0;
}
