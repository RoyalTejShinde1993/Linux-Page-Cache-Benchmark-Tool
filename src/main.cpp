#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include "benchmark.hpp"

static void printUsage() {
    std::cout << "Usage: ./pagebench --mode <buffered|direct|mmap> --file <path> [options]\n"
                 "Options:\n"
                 "  --mode <mode>          Benchmark mode: buffered, direct, mmap\n"
                 "  --file <path>          Input file path\n"
                 "  --block-size <bytes>   Read block size (default 4096)\n"
                 "  --bytes <bytes>        Maximum bytes to read from file\n"
                 "  -h, --help             Show this help message\n";
}

static bool fileExists(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return file.good();
}

static bool parseSize(const std::string& token, std::size_t& value) {
    if (token.empty())
        return false;

    std::string digits = token;
    unsigned long long multiplier = 1;
    char suffix = static_cast<char>(digits.back());

    if (suffix == 'K' || suffix == 'k') {
        multiplier = 1024ULL;
        digits.pop_back();
    } else if (suffix == 'M' || suffix == 'm') {
        multiplier = 1024ULL * 1024ULL;
        digits.pop_back();
    } else if (suffix == 'G' || suffix == 'g') {
        multiplier = 1024ULL * 1024ULL * 1024ULL;
        digits.pop_back();
    }

    if (digits.empty())
        return false;

    for (char c : digits) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    try {
        unsigned long long number = std::stoull(digits);
        unsigned long long result = number * multiplier;
        if (result / multiplier != number || result > std::numeric_limits<std::size_t>::max())
            return false;
        value = static_cast<std::size_t>(result);
        return true;
    } catch (...) {
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        }
    }

    std::string mode;
    std::string file;
    std::size_t blockSize = 4096;
    std::size_t maxBytes = SIZE_MAX;

    if (argc == 3 && argv[1][0] != '-') {
        mode = argv[1];
        file = argv[2];
    } else {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                printUsage();
                return 0;
            }

            if (arg == "--mode" && i + 1 < argc) {
                mode = argv[++i];
            } else if (arg == "--file" && i + 1 < argc) {
                file = argv[++i];
            } else if (arg == "--block-size" && i + 1 < argc) {
                if (!parseSize(argv[++i], blockSize) || blockSize == 0) {
                    std::cerr << "Invalid block size: " << argv[i] << "\n";
                    return 1;
                }
            } else if (arg == "--bytes" && i + 1 < argc) {
                if (!parseSize(argv[++i], maxBytes) || maxBytes == 0) {
                    std::cerr << "Invalid bytes value: " << argv[i] << "\n";
                    return 1;
                }
            } else {
                std::cerr << "Unknown argument: " << arg << "\n";
                printUsage();
                return 1;
            }
        }
    }

    if (mode.empty() || file.empty()) {
        printUsage();
        return 1;
    }

    if (!fileExists(file)) {
        std::cerr << "File not found: " << file << "\n";
        return 1;
    }

    if (mode == "buffered") {
        runBufferedBenchmark(file, blockSize, maxBytes);
    } else if (mode == "direct") {
        runDirectBenchmark(file, blockSize, maxBytes);
    } else if (mode == "mmap") {
        runMmapBenchmark(file, blockSize, maxBytes);
    } else {
        std::cerr << "Unknown mode: " << mode << "\n";
        printUsage();
        return 1;
    }

    return 0;
}
