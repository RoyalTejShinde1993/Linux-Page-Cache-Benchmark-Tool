#pragma once

#include <cstddef>
#include <string>

void runBufferedBenchmark(const std::string& path,
                          std::size_t blockSize = 4096,
                          std::size_t maxBytes = SIZE_MAX);
void runDirectBenchmark(const std::string& path,
                        std::size_t blockSize = 4096,
                        std::size_t maxBytes = SIZE_MAX);
void runMmapBenchmark(const std::string& path,
                      std::size_t blockSize = 4096,
                      std::size_t maxBytes = SIZE_MAX);
