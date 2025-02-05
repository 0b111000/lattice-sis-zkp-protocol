#include "test_utils.hpp"

namespace test {
    void run_basic_tests();
    void run_performance_tests();
}

int main() {
    try {
        // Initialize random seed
        NTL::SetSeed(NTL::conv<NTL::ZZ>(time(0)));
        
        // Run all tests
        test::run_basic_tests();
        test::run_performance_tests();
        
        std::cout << "\nAll tests completed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}