#pragma once

#include "protocol/lattice_proof.hpp"
#include <chrono>
#include <iostream>
#include <cassert>

namespace test {

using Clock = std::chrono::high_resolution_clock;
using Nanoseconds = std::chrono::nanoseconds;

// Structure for proof size analysis
struct ProofSize {
    size_t commitment_size;    // Size of u
    size_t challenge_size;     // Size of c
    size_t response_size;      // Size of z
    size_t total_size;         // Total proof size

    void print() const {
        double kb_size = total_size / 8.0 / 1024.0;
        double mb_size = kb_size / 1024.0;
        
        std::cout << "Proof Size Analysis:\n"
                  << "  Commitment size: " << commitment_size << " bits"
                  << " (" << (commitment_size / 8.0 / 1024.0) << " KB)\n"
                  << "  Challenge size: " << challenge_size << " bits"
                  << " (" << (challenge_size / 8.0 / 1024.0) << " KB)\n"
                  << "  Response size: " << response_size << " bits"
                  << " (" << (response_size / 8.0 / 1024.0) << " KB)\n"
                  << "  Total proof size: " << total_size << " bits"
                  << " (" << kb_size << " KB, " << mb_size << " MB)\n";
    }
};
// Structure for benchmarking results
struct BenchmarkResult {
    Nanoseconds setup_time;
    Nanoseconds commit_time;
    Nanoseconds challenge_time;
    Nanoseconds response_time;
    Nanoseconds verify_time;
    Nanoseconds total_time;
    ProofSize proof_size;
    
    void print() const {
        std::cout << "Time Measurements:\n"
                  << "  Setup time: " << setup_time.count() << " ns"
                  << " (" << (setup_time.count() / 1e6) << " ms)\n"
                  << "  Commit time: " << commit_time.count() << " ns"
                  << " (" << (commit_time.count() / 1e6) << " ms)\n"
                  << "  Challenge time: " << challenge_time.count() << " ns"
                  << " (" << (challenge_time.count() / 1e6) << " ms)\n"
                  << "  Response time: " << response_time.count() << " ns"
                  << " (" << (response_time.count() / 1e6) << " ms)\n"
                  << "  Verify time: " << verify_time.count() << " ns"
                  << " (" << (verify_time.count() / 1e6) << " ms)\n"
                  << "  Total time: " << total_time.count() << " ns"
                  << " (" << (total_time.count() / 1e6) << " ms)\n\n";
        proof_size.print();
    }
};

// Utility function to calculate proof sizes
ProofSize calculate_proof_size(const protocol::Parameters& params, 
                             const NTL::vec_ZZ_p& u,
                             const NTL::vec_ZZ& challenge,
                             const NTL::vec_ZZ& z);

} // namespace test