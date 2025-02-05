#include "protocol/parameters.hpp"
#include <sstream>
#include <stdexcept>

namespace protocol {

bool is_prime(const NTL::ZZ& n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    NTL::ZZ sqrtn = SqrRoot(n);
    for (NTL::ZZ i = NTL::ZZ(3); i <= sqrtn; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

Parameters::Parameters(int n, int m, const NTL::ZZ& q, 
                     int y_range, int s_range, 
                     double safety_factor,
                     double sigma)
    : n_(n), m_(m), q_(q), 
      y_range_(y_range), s_range_(s_range), 
      safety_factor_(safety_factor), sigma_(sigma) {
    if (!validate()) {
        throw std::invalid_argument("Invalid parameters");
    }
}

Parameters Parameters::DefaultParams() {
    return Parameters(
        4,      // n
        4,      // m
        NTL::conv<NTL::ZZ>(97),  // q
        10,     // y_range
        1,      // s_range
        10.0,   // safety_factor
        1.5     // sigma
    );
}

Parameters Parameters::HighSecurityParams() {
    return Parameters(
        512,    // n
        512,    // m
        NTL::conv<NTL::ZZ>("4294967291"),  // Prime close to 2^32 (4294967296)
        10,     // y_range
        1,      // s_range
        10.0,   // safety_factor
        1.5     // sigma
    );
}

bool Parameters::validate() const {
    if (n_ <= 0 || m_ <= 0) {
        throw std::invalid_argument("Dimensions must be positive");
    }
    if (q_ <= 0) {
        throw std::invalid_argument("Modulus must be positive");
    }
    if (y_range_ <= 0 || s_range_ <= 0) {
        throw std::invalid_argument("Ranges must be positive");
    }
    if (safety_factor_ <= 0) {
        throw std::invalid_argument("Safety factor must be positive");
    }
    if (sigma_ <= 0) {
        throw std::invalid_argument("Sigma must be positive");
    }
    if (!is_prime(q_)) {
        throw std::invalid_argument("Modulus must be prime");
    }
    return true;
}

std::string Parameters::toString() const {
    std::stringstream ss;
    ss << "Parameters:\n"
       << "  n = " << n_ << "\n"
       << "  m = " << m_ << "\n"
       << "  q = " << q_ << " (bits: " << NTL::NumBits(q_) << ")\n"
       << "  y_range = " << y_range_ << "\n"
       << "  s_range = " << s_range_ << "\n"
       << "  safety_factor = " << safety_factor_ << "\n"
       << "  sigma = " << sigma_ << "\n";
    return ss.str();
}

} // namespace protocol