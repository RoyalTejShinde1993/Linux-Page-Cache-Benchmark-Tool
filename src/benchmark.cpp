#include <iomanip>
#include <iostream>

void printResult(const std::string& name,
                 double seconds,
                 size_t bytes) {
    double mb = bytes / (1024.0 * 1024.0);
    double throughput = (seconds > 0.0) ? (mb / seconds) : 0.0;

    std::cout << "\n=== " << name << " ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Processed: " << mb << " MB\n";
    std::cout << "Time: " << seconds << " sec\n";
    std::cout << "Throughput: " << throughput << " MB/s\n";
}
