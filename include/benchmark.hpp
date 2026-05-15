#pragma once

#include <string>

void runBufferedBenchmark(const std::string& path);
void runDirectBenchmark(const std::string& path);
void runMmapBenchmark(const std::string& path);
