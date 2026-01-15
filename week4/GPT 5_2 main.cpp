
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <iomanip>
#include <chrono>

// Using dynamic chunking to balance load between Performance and Efficiency cores on M1.
// Chunk size chosen to balance atomic overhead vs load distribution.
constexpr int CHUNK_SIZE = 50000; 

// Atomic counter for work stealing. Starts at 1 as per Python range(1, ...).
std::atomic<int> g_counter(1);

// Worker thread function
void worker(int iterations, double param1, double param2, double* result_out) {
    double local_total = 0.0;
    
    // Cache parameters in registers
    const double p1 = param1;
    const double p2 = param2;
    
    while (true) {
        // Fetch next block of work
        int start = g_counter.fetch_add(CHUNK_SIZE, std::memory_order_relaxed);
        if (start > iterations) {
            break;
        }
        
        int end = std::min(start + CHUNK_SIZE, iterations + 1);
        
        // Accumulators for unrolled loop to maximize instruction level parallelism
        double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
        
        int i = start;
        int limit = end - 3;
        
        // 4-way manual unrolling to utilize multiple FP pipelines
        for (; i < limit; i += 4) {
            // Iteration i
            double iv1 = (double)i;
            double j1_m = iv1 * p1 - p2;
            double j1_p = iv1 * p1 + p2;
            s1 -= 1.0 / j1_m;
            s1 += 1.0 / j1_p;

            // Iteration i+1
            double iv2 = (double)(i + 1);
            double j2_m = iv2 * p1 - p2;
            double j2_p = iv2 * p1 + p2;
            s2 -= 1.0 / j2_m;
            s2 += 1.0 / j2_p;

            // Iteration i+2
            double iv3 = (double)(i + 2);
            double j3_m = iv3 * p1 - p2;
            double j3_p = iv3 * p1 + p2;
            s3 -= 1.0 / j3_m;
            s3 += 1.0 / j3_p;

            // Iteration i+3
            double iv4 = (double)(i + 3);
            double j4_m = iv4 * p1 - p2;
            double j4_p = iv4 * p1 + p2;
            s4 -= 1.0 / j4_m;
            s4 += 1.0 / j4_p;
        }
        
        // Process remaining items in the chunk
        for (; i < end; ++i) {
            double iv = (double)i;
            double j_m = iv * p1 - p2;
            double j_p = iv * p1 + p2;
            s1 -= 1.0 / j_m;
            s1 += 1.0 / j_p;
        }
        
        local_total += (s1 + s2 + s3 + s4);
    }
    
    *result_out = local_total;
}

int main() {
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    const int iterations = 200000000;
    const double param1 = 4.0;
    const double param2 = 1.0;
    
    // Detect hardware threads (M1 typically has 8)
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 8;
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    std::vector<double> results(num_threads, 0.0);
    
    // Launch worker threads
    for (unsigned int t = 0; t < num_threads; ++t) {
        threads.emplace_back(worker, iterations, param1, param2, &results[t]);
    }
    
    // Wait for completion
    for (auto& t : threads) {
        t.join();
    }
    
    // Aggregate partial results
    // Initial result is 1.0 as per Python code
    double final_result = 1.0;
    for (double r : results) {
        final_result += r;
    }
    
    final_result *= 4.0;
    
    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // Output formatted results
    std::cout << "Result: " << std::fixed << std::setprecision(12) << final_result << std::endl;
    std::cout << "Execution Time: " << std::fixed << std::setprecision(6) << diff.count() << " seconds" << std::endl;

    return 0;
}
