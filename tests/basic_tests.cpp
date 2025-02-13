#include "test_utils.hpp"
#include <vector>

namespace test {

// Basic functionality test (already present)
void test_basic_functionality() {
    std::cout << "\nTest: Basic Functionality\n";
    
    auto params = protocol::Parameters::DefaultParams();
    std::cout << params.toString();
    
    protocol::LatticeProof proof(params);
    auto u = proof.commit();
    auto challenge = protocol::LatticeProof::generate_challenge(params.m());
    auto z = proof.respond(challenge);
    bool valid = proof.verify(u, challenge, z);
    
    assert(valid && "Basic functionality verification failed");
    std::cout << "✓ Basic functionality test passed\n";
}

// Multiple proof iterations test (was missing)
void test_multiple_proofs() {
    std::cout << "\nTest: Multiple Proof Iterations\n";
    
    auto params = protocol::Parameters::DefaultParams();
    protocol::LatticeProof proof(params);
    
    for (int i = 0; i < 10; i++) {
        std::cout << "  Iteration " << i + 1 << "/10\n";
        auto u = proof.commit();
        auto challenge = protocol::LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        bool valid = proof.verify(u, challenge, z);
        assert(valid && "Multiple proofs test failed");
    }
    
    std::cout << "✓ Multiple proofs test passed\n";
}

// Parameter size test (was missing)
void test_parameter_sizes() {
    std::cout << "\nTest: Different Parameter Sizes\n";
    
    std::vector<std::pair<int, int>> sizes = {
        {4, 4}, {8, 8}, {16, 16}, {32, 32}, {64, 64}
    };
    
    for (const auto& [n, m] : sizes) {
        std::cout << "  Testing size n=" << n << ", m=" << m << "\n";
        protocol::Parameters params(n, m, NTL::conv<NTL::ZZ>(97));
        protocol::LatticeProof proof(params);
        
        auto u = proof.commit();
        auto challenge = protocol::LatticeProof::generate_challenge(m);
        auto z = proof.respond(challenge);
        bool valid = proof.verify(u, challenge, z);
        
        assert(valid && "Parameter size test failed");
    }
    
    std::cout << "✓ Parameter sizes test passed\n";
}

// Enhanced validation tests with more corner cases
void test_validation() {
    std::cout << "\nTest: Validation and Corner Cases\n";
    
    // Test 1: Invalid dimensions
    try {
        protocol::Parameters params(0, 4, NTL::conv<NTL::ZZ>(97));
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument&) {
        std::cout << "✓ Invalid dimension check passed\n";
    }
    
    // Test 2: Non-prime modulus
    std::cout << "  Testing non-prime modulus...\n";
    try {
        protocol::Parameters params(4, 4, NTL::conv<NTL::ZZ>(100));  // 100 is clearly not prime
        assert(false && "Should have thrown exception for non-prime modulus");
    } catch (const std::invalid_argument& e) {
        std::cout << "✓ Non-prime modulus check passed: " << e.what() << "\n";
    }
    
    auto params = protocol::Parameters::DefaultParams();
    protocol::LatticeProof proof(params);
    
    // Test 3: Tampered response
    {
        auto u = proof.commit();
        auto challenge = protocol::LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        z[0] = (z[0] + 1) % params.q();
        bool valid = proof.verify(u, challenge, z);
        assert(!valid && "Tampered response check failed");
        std::cout << "✓ Tampered response check passed\n";
    }
    
    // Test 4: Wrong size challenge
    {
        auto u = proof.commit();
        auto wrong_challenge = protocol::LatticeProof::generate_challenge(params.m() + 1);
        try {
            auto z = proof.respond(wrong_challenge);
            assert(false && "Should have thrown exception");
        } catch (const std::invalid_argument&) {
            std::cout << "✓ Wrong challenge size check passed\n";
        }
    }
    
    // Test 5: Zero challenge vector
    {
        auto u = proof.commit();
        NTL::vec_ZZ zero_challenge;
        zero_challenge.SetLength(params.m());
        for (long i = 0; i < params.m(); i++) {
            zero_challenge[i] = 0;
        }
        auto z = proof.respond(zero_challenge);
        bool valid = proof.verify(u, zero_challenge, z);
        assert(valid && "Zero challenge verification failed");
        std::cout << "✓ Zero challenge check passed\n";
    }
    
    // Test 6: Maximum value challenge
    {
        auto u = proof.commit();
        NTL::vec_ZZ max_challenge;
        max_challenge.SetLength(params.m());
        for (long i = 0; i < params.m(); i++) {
            max_challenge[i] = 1;  // Maximum allowed in {-1,0,1}
        }
        auto z = proof.respond(max_challenge);
        bool valid = proof.verify(u, max_challenge, z);
        assert(valid && "Maximum challenge verification failed");
        std::cout << "✓ Maximum challenge check passed\n";
    }
    
    // Test 7: Repeated proofs with same commitment (should actually work!)
    {
        std::cout << "  Testing commitment reuse...\n";
        auto u = proof.commit();
        for (int i = 0; i < 5; i++) {
            auto challenge = protocol::LatticeProof::generate_challenge(params.m());
            auto z = proof.respond(challenge);
            bool valid = proof.verify(u, challenge, z);
            assert(valid && "Valid proof with reused commitment failed to verify");
        }
        std::cout << "✓ Commitment reuse verification passed\n";
    }

    // test for commitment tampering instead:
    {
        std::cout << "  Testing commitment tampering...\n";
        auto u = proof.commit();
        auto challenge = protocol::LatticeProof::generate_challenge(params.m());
        auto z = proof.respond(challenge);
        
        // Tamper with the commitment
        NTL::vec_ZZ_p tampered_u;
        tampered_u.SetLength(u.length());
        for (long i = 0; i < u.length(); i++) {
            tampered_u[i] = u[i] + NTL::conv<NTL::ZZ_p>(1);  // Add 1 to each component
        }
        
        bool valid = proof.verify(tampered_u, challenge, z);
        assert(!valid && "Tampered commitment was incorrectly verified");
        std::cout << "✓ Commitment tampering check passed\n";
    }
}

// Run all basic tests
void run_basic_tests() {
    test_basic_functionality();
    test_multiple_proofs();
    test_parameter_sizes();
    test_validation();
}

} // namespace test