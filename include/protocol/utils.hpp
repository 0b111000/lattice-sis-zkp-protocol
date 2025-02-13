#pragma once
#include "parameters.hpp"  // Add this include

#include <NTL/ZZ.h>
#include <NTL/vec_ZZ.h>
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <string>

namespace protocol {

// Sampling functions
NTL::vec_ZZ sample_ternary(int length);
NTL::vec_ZZ sample_uniform(int length, long bound);

// Vector operations
NTL::vec_ZZ_p matrix_vector_mod(const NTL::mat_ZZ_p& M, const NTL::vec_ZZ& v);
NTL::ZZ compute_norm_squared(const NTL::vec_ZZ& v, const NTL::ZZ& q);

// Norm calculations
long calculate_norm_bound(int m, int y_range, int s_range, double safety_factor = 10.0);
double calculate_expected_y_contribution(int m, int y_range);
double calculate_expected_s_contribution(int m, int s_range);

// Challenge generation
NTL::vec_ZZ generate_challenge(int length);

// Debug utilities
void print_vector(const std::string& label, const NTL::vec_ZZ& v);
void print_matrix(const std::string& label, const NTL::mat_ZZ_p& M);


// Add to utils.hpp
void validate_dimensions(const Parameters& params,
                       const NTL::vec_ZZ_p& u,
                       const NTL::vec_ZZ& challenge,
                       const NTL::vec_ZZ& z);

} // namespace protocol