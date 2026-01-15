
#include <iostream>
#include <iomanip>
#include <chrono>

/**
 * Ported Python code to high-performance C++.
 * 
 * Optimized for Apple M1 using clang++ with -Ofast, -mcpu=native, and -flto.
 * The implementation maintains the logic of the Leibniz series calculation while
 * allowing the compiler to leverage NEON SIMD instructions and multiple 
 * floating-point units on the M1 processor.
 */

int main() {
    // Synchronize timing with the start of calculation
    auto start_time = std::chrono::steady_clock::now();

    // Constant parameters taken from the Python script
    const long long iterations = 200000000;
    const double p1 = 4.0;
    const double p2 = 1.0;
    
    // Initial result value
    double result = 1.0;

    /**
     * Computational Loop:
     * This loop is structured to be easily vectorized by the compiler.
     * On the Apple M1, -Ofast allows the compiler to use multiple accumulators
     * to break the dependency chain on the 'result' variable, significantly
     * increasing throughput by utilizing the 4 available floating-point pipelines.
     */
    for (long long i = 1; i <= iterations; ++i) {
        // Calculate the two denominators for the current iteration
        double current_base = (double)i * p1;
        
        // Subtract 1 / (4*i - 1)
        result -= 1.0 / (current_base - p2);
        
        // Add 1 / (4*i + 1)
        result += 1.0 / (current_base + p2);
    }

    // Multiply the final sum by 4 to get the result (approx. pi)
    result *= 4.0;

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // Formatting output to match Python's f-string results exactly
    // Result: .12f
    // Execution Time: .6f
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Result: " << result << "\n";
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Execution Time: " << diff.count() << " seconds" << std::endl;

    return 0;
}

