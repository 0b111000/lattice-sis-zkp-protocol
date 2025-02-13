#include "protocol/utils.hpp"
#include <cmath>
#include <iostream>

namespace protocol {

NTL::vec_ZZ sample_ternary(int length) {
    NTL::vec_ZZ result;
    result.SetLength(length);
    for (int i = 0; i < length; i++) {
        result[i] = NTL::RandomBnd(3) - 1;  // {-1, 0, 1}
    }
    return result;
}

NTL::vec_ZZ sample_uniform(int length, long bound) {
    NTL::vec_ZZ result;
    result.SetLength(length);
    for (int i = 0; i < length; i++) {
        result[i] = NTL::RandomBnd(2 * bound + 1) - bound;  // [-bound, bound]
    }
    return result;
}

NTL::vec_ZZ_p matrix_vector_mod(const NTL::mat_ZZ_p& M, const NTL::vec_ZZ& v) {
    NTL::vec_ZZ_p v_mod;
    v_mod.SetLength(v.length());
    for (long i = 0; i < v.length(); i++) {
        v_mod[i] = NTL::conv<NTL::ZZ_p>(v[i]);  // Convert directly to ZZ_p
    }
    return M * v_mod;  // Matrix-vector multiplication (mod q is implicit)
}

NTL::ZZ compute_norm_squared(const NTL::vec_ZZ& v, const NTL::ZZ& q) {
    NTL::ZZ norm_sq = NTL::conv<NTL::ZZ>(0);
    for (long i = 0; i < v.length(); i++) {
        NTL::ZZ vi = v[i];
        if (vi > q/2) vi -= q;
        norm_sq += vi * vi;
    }
    return norm_sq;
}

long calculate_norm_bound(int m, int y_range, int s_range, double safety_factor) {
    double E_y_squared = calculate_expected_y_contribution(m, y_range);
    double E_s_squared = calculate_expected_s_contribution(m, s_range);
    double expected_norm_squared = E_y_squared + E_s_squared;
    return static_cast<long>(ceil(safety_factor * expected_norm_squared));
}

double calculate_expected_y_contribution(int m, int y_range) {
    return m * (pow(y_range, 2) - 1) / 3.0;
}

double calculate_expected_s_contribution(int m, int s_range) {
    return m * pow(s_range, 2);
}

NTL::vec_ZZ generate_challenge(int length) {
    NTL::vec_ZZ c;
    c.SetLength(length);
    for (int i = 0; i < length; i++) {
        c[i] = NTL::RandomBnd(3) - 1;  // {-1, 0, 1}
    }
    return c;
}

void print_vector(const std::string& label, const NTL::vec_ZZ& v) {
    std::cout << label << ": [";
    for (long i = 0; i < v.length(); i++) {
        std::cout << v[i];
        if (i < v.length() - 1) std::cout << " ";
    }
    std::cout << "]" << std::endl;
}

void print_matrix(const std::string& label, const NTL::mat_ZZ_p& M) {
    std::cout << label << ":\n";
    for (long i = 0; i < M.NumRows(); i++) {
        std::cout << "[";
        for (long j = 0; j < M.NumCols(); j++) {
            std::cout << NTL::conv<long>(rep(M[i][j]));
            if (j < M.NumCols() - 1) std::cout << " ";
        }
        std::cout << "]\n";
    }
}

void validate_dimensions(const Parameters& params,
                       const NTL::vec_ZZ_p& u,
                       const NTL::vec_ZZ& challenge,
                       const NTL::vec_ZZ& z) {
    if (u.length() != params.n()) {
        throw std::invalid_argument("Commitment vector has wrong dimension");
    }
    if (challenge.length() != params.m()) {
        throw std::invalid_argument("Challenge vector has wrong dimension");
    }
    if (z.length() != params.m()) {
        throw std::invalid_argument("Response vector has wrong dimension");
    }
}

} // namespace protocol