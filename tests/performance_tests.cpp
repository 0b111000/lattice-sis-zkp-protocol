#include "test_utils.hpp"
#include <vector>
#include <tuple>

namespace test {

// Implementation of calculate_proof_size that was declared in test_utils.hpp
ProofSize calculate_proof_size(const protocol::Parameters& params,
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

// Implementation of benchmark_protocol
BenchmarkResult benchmark_protocol(const protocol::Parameters& params) {
    BenchmarkResult result;
    auto start_total = Clock::now();
    
    // Setup
    auto start = Clock::now();
    protocol::LatticeProof proof(params);
    auto end = Clock::now();
    result.setup_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Commit
    start = Clock::now();
    auto u = proof.commit();
    end = Clock::now();
    result.commit_time = std::chrono::duration_cast<Nanoseconds>(end - start);
    
    // Challenge
    start = Clock::now();
    auto challenge = protocol::LatticeProof::generate_challenge(params.m());
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

void run_performance_tests() {
    std::cout << "\nRunning Performance Tests with Real-World Parameters\n";
    
    // Parameters matching paper's recommendations with prime moduli
    // Using primes close to but less than power of 2
    std::vector<std::tuple<int, std::string, NTL::ZZ>> sizes = {
        {512, "Standard", 
         NTL::conv<NTL::ZZ>("1073741789")},  // Prime close to 2^30 (1073741824)
        {1024, "High Security", 
         NTL::conv<NTL::ZZ>("4294967291")}   // Prime close to 2^32 (4294967296)
    };
    
    for (const auto& [size, label, q] : sizes) {
        std::cout << "\n" << label << " Parameter Set:\n";
        std::cout << "n = m = " << size << ", log(q) = " << NTL::NumBits(q) << " bits\n";
        
        try {
            // Create parameters with appropriate sigma value
            protocol::Parameters params(
                size,   // n
                size,   // m
                q,      // q (prime)
                10,     // y_range
                1,      // s_range
                10.0,   // safety_factor
                1.5     // sigma (as used in the paper)
            );
            
            auto result = benchmark_protocol(params);
            result.print();
            
            // Calculate theoretical size as per paper
            size_t theoretical_size = 0;
            
            // Commitment: n log q bits
            theoretical_size += size * NTL::NumBits(q);
            
            // Challenge: κ bits (security parameter, typically 256)
            theoretical_size += 256;
            
            // Response: n log(2σ√n) bits
            double sigma = params.sigma();
            double log_response = std::log2(2 * sigma * std::sqrt(size));
            theoretical_size += size * static_cast<size_t>(std::ceil(log_response));
            
            std::cout << "Theoretical size (as per paper): " 
                     << theoretical_size << " bits ("
                     << (theoretical_size / 8.0 / 1024.0) << " KB)\n";
                     
        } catch (const std::exception& e) {
            std::cerr << "Error with " << label << " parameters: " << e.what() << "\n";
        }
    }
}

} // namespace test