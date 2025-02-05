#include "protocol/lattice_proof.hpp"
#include "protocol/parameters.hpp"
#include "protocol/utils.hpp"
#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>

using namespace protocol;
using Clock = std::chrono::high_resolution_clock;
using Nanoseconds = std::chrono::nanoseconds;

struct ProofSize {
    size_t commitment_size;    // Size of u
    size_t challenge_size;     // Size of c
    size_t response_size;      // Size of z
    size_t total_size;         // Total proof size

    void print() const {
        std::cout << "Proof Size Analysis:\n"
                  << "  Commitment size: " << commitment_size << " bits\n"
                  << "  Challenge size: " << challenge_size << " bits\n"
                  << "  Response size: " << response_size << " bits\n"
                  << "  Total proof size: " << total_size << " bits\n"
                  << "  Total size in KB: " << (total_size / 8.0 / 1024.0) << " KB\n";
    }
};

// Benchmark structure
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

// Function to calculate proof sizes
ProofSize calculate_proof_size(const Parameters& params, 
                             const NTL::vec_ZZ_p& u,
                             const NTL::vec_ZZ& challenge,
                             const NTL::vec_ZZ& z) {
    ProofSize size;
    
    // Calculate bit sizes
    long q_bits = NTL::NumBits(params.q());
    
    // Commitment size: n elements mod q
    size.commitment_size = params.n() * q_bits;
    
    // Challenge size: m elements from {-1,0,1}, so 2 bits each
    size.challenge_size = params.m() * 2;
    
    // Response size: m elements mod q
    size.response_size = params.m() * q_bits;
    
    // Total size
    size.total_size = size.commitment_size + size.challenge_size + size.response_size;
    
    return size;
}

// Basic functionality tests
void test_small_dimension() {
    std::cout << "\nTest: Basic functionality with small dimensions\n";
    
    auto params = Parameters::DefaultParams();
    std::cout << params.toString();
    
    LatticeProof proof(params);
    auto u = proof.commit();
    auto challenge = LatticeProof::generate_challenge(params.m());
    auto z = proof.respond(challenge);
    bool valid = proof.verify(u, challenge, z);
    
    assert(valid && "Small dimension verification failed");
    std::cout << "✓ Basic functionality test passed\n";
}

// Test multiple iterations
void test_multiple_proofs(int iterations = 10) {
    std::cout << "\nTest: Multiple proof iterations\n";
    
    auto params = Parameters::DefaultParams();
    LatticeProof proof(params);
    
    for (int i = 0; i < iterations; i++) {
        auto u = proof.commit();
        auto challenge = LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        bool valid = proof.verify(u, challenge, z);
        assert(valid && "Multiple proofs test failed");
    }
    
    std::cout << "✓ Successfully completed " << iterations << " proof iterations\n";
}

// Test different parameter sizes
void test_parameter_sizes() {
    std::cout << "\nTest: Different parameter sizes\n";
    
    std::vector<std::pair<int, int>> sizes = {
        {4, 4}, {8, 8}, {16, 16}, {32, 32}, {64, 64}
    };
    
    for (const auto& [n, m] : sizes) {
        std::cout << "Testing size n=" << n << ", m=" << m << "\n";
        Parameters params(n, m, NTL::conv<NTL::ZZ>(97));
        LatticeProof proof(params);
        
        auto u = proof.commit();
        auto challenge = LatticeProof::generate_challenge(m);
        auto z = proof.respond(challenge);
        bool valid = proof.verify(u, challenge, z);
        
        assert(valid && "Parameter size test failed");
    }
    
    std::cout << "✓ All parameter sizes tested successfully\n";
}

void test_invalid_proofs() {
    std::cout << "\nTest: Invalid proof detection\n";
    
    auto params = Parameters::DefaultParams();
    LatticeProof proof(params);
    
    // Test 1: Tampered response
    {
        std::cout << "  Testing tampered response...\n";
        auto u = proof.commit();
        auto challenge = LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        z[0] = (z[0] + 1) % params.q();  // Tamper with response
        bool valid = proof.verify(u, challenge, z);
        assert(!valid && "Failed to detect tampered response");
        std::cout << "  ✓ Tampered response detected\n";
    }
    
    // Test 2: Wrong challenge size
    {
        std::cout << "  Testing wrong challenge size...\n";
        auto u = proof.commit();
        auto wrong_challenge = LatticeProof::generate_challenge(params.m() + 1);
        bool exception_caught = false;
        try {
            auto z = proof.respond(wrong_challenge);
        } catch (const std::invalid_argument& e) {
            exception_caught = true;
            std::cout << "  ✓ Caught expected exception: " << e.what() << "\n";
        }
        assert(exception_caught && "Failed to catch wrong challenge size");
    }
    
    // Test 3: Wrong commitment size
    {
        std::cout << "  Testing wrong commitment size...\n";
        auto u = proof.commit();
        auto challenge = LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        
        // Create wrong-sized commitment
        NTL::vec_ZZ_p wrong_u;
        wrong_u.SetLength(params.n() + 1);
        for (long i = 0; i < params.n() + 1; i++) {
            wrong_u[i] = NTL::random_ZZ_p();
        }
        
        bool exception_caught = false;
        try {
            bool valid = proof.verify(wrong_u, challenge, z);
        } catch (const std::invalid_argument& e) {
            exception_caught = true;
            std::cout << "  ✓ Caught expected exception: " << e.what() << "\n";
        }
        assert(exception_caught && "Failed to catch wrong commitment size");
    }
    
    std::cout << "✓ All invalid proof detection tests passed\n";
}

// Performance benchmarking
BenchmarkResult benchmark_protocol(const Parameters& params) {
    BenchmarkResult result;
    auto start_total = Clock::now();
    
    // Setup
    auto start = Clock::now();
    LatticeProof proof(params);
    auto end = Clock::now();
    result.setup_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Commit
    start = Clock::now();
    auto u = proof.commit();
    end = Clock::now();
    result.commit_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Challenge
    start = Clock::now();
    auto challenge = LatticeProof::generate_challenge(params.m());
    end = Clock::now();
    result.challenge_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Response
    start = Clock::now();
    auto z = proof.respond(challenge);
    end = Clock::now();
    result.response_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Verify
    start = Clock::now();
    bool valid = proof.verify(u, challenge, z);
    end = Clock::now();
    result.verify_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    assert(valid && "Benchmark proof verification failed");
    
    auto end_total = Clock::now();
    result.total_time = std::chrono::duration_cast<Nanoseconds>(end_total - start_total);
    
    // Calculate proof sizes
    result.proof_size = calculate_proof_size(params, u, challenge, z);
    
    return result;
}


void run_benchmarks() {
    std::cout << "\nRunning Performance Benchmarks and Size Analysis\n";
    
    // Test different sizes
    std::vector<std::tuple<int, std::string, NTL::ZZ>> sizes = {
        {32, "Small", NTL::conv<NTL::ZZ>(8191)},          // 13-bit prime
        {64, "Medium", NTL::conv<NTL::ZZ>(21023)},        // 15-bit prime
        {128, "Large", NTL::conv<NTL::ZZ>(65537)},        // 16-bit prime
        {256, "Very Large", NTL::conv<NTL::ZZ>(131071)},  // 17-bit prime
        {512, "Extreme", NTL::conv<NTL::ZZ>(524287)}      // 19-bit prime
    };
    
    for (const auto& [size, label, q] : sizes) {
        std::cout << "\n" << label << " Parameter Set:\n";
        std::cout << "n = m = " << size << ", log(q) = " << NTL::NumBits(q) << " bits\n";
        
        Parameters params(size, size, q);
        auto result = benchmark_protocol(params);
        result.print();
        
        // Optional: Calculate theoretical communication complexity
        size_t theoretical_size = size * (NTL::NumBits(q) + 2 + NTL::NumBits(q));
        std::cout << "Theoretical communication complexity: " 
                  << theoretical_size << " bits ("
                  << (theoretical_size / 8.0 / 1024.0) << " KB)\n";
    }
}

int main() {
    try {
        // Initialize random seed
        NTL::SetSeed(NTL::conv<NTL::ZZ>(time(0)));
        
        // Run functionality tests
        test_small_dimension();
        test_multiple_proofs();
        test_parameter_sizes();
        test_invalid_proofs();
        
        // Run benchmarks
        run_benchmarks();
        
        std::cout << "\nAll tests and benchmarks completed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}