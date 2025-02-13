#pragma once

#include <NTL/ZZ.h>
#include <string>

namespace protocol {

class Parameters {
public:
    Parameters(int n, int m, const NTL::ZZ& q, 
               int y_range = 10, int s_range = 1, 
               double safety_factor = 10.0,
               double sigma = 1.5);  // Added sigma parameter
    
    static Parameters DefaultParams();
    static Parameters HighSecurityParams();
    
    // Getters
    int n() const { return n_; }
    int m() const { return m_; }
    const NTL::ZZ& q() const { return q_; }
    int y_range() const { return y_range_; }
    int s_range() const { return s_range_; }
    double safety_factor() const { return safety_factor_; }
    double sigma() const { return sigma_; }  // Added getter for sigma
    
    bool validate() const;
    std::string toString() const;

private:
    int n_;              // lattice dimension
    int m_;              // vector dimension
    NTL::ZZ q_;         // modulus
    int y_range_;       // range for uniform sampling
    int s_range_;       // range for ternary sampling
    double safety_factor_; // safety factor for norm bound
    double sigma_;      // Gaussian parameter
};

} // namespace protocol