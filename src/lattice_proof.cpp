#include "protocol/lattice_proof.hpp"
#include <iostream>

namespace protocol {

LatticeProof::LatticeProof(const Parameters& params)
    : params_(params) {
    // Initialize ZZ_p context with modulus q
    NTL::ZZ_p::init(params_.q());

    // Generate random matrix A
    A_.SetDims(params_.n(), params_.m());
    for (int i = 0; i < params_.n(); i++) {
        for (int j = 0; j < params_.m(); j++) {
            A_[i][j] = NTL::random_ZZ_p();
        }
    }

    // Sample secret s from {-1,0,1}
    s_ = sample_ternary(params_.m());

    // Compute public value t = As mod q
    t_ = matrix_vector_mod(A_, s_);
}

NTL::vec_ZZ_p LatticeProof::commit() {
    // Sample random y with small norm
    y_ = sample_uniform(params_.m(), params_.y_range());

    // Compute commitment u = Ay mod q
    return matrix_vector_mod(A_, y_);
}

NTL::vec_ZZ LatticeProof::respond(const NTL::vec_ZZ& challenge) {
    // Add size validation
    if (challenge.length() != params_.m()) {
        throw std::invalid_argument("Challenge vector has wrong dimension");
    }
    NTL::vec_ZZ z;
    z.SetLength(params_.m());

    // Compute z = y + cs
    for (int i = 0; i < params_.m(); i++) {
        z[i] = y_[i] + challenge[i] * s_[i];
        z[i] = (z[i] % params_.q() + params_.q()) % params_.q();
    }

    return z;
}

bool LatticeProof::verify(const NTL::vec_ZZ_p& u, 
                         const NTL::vec_ZZ& challenge, 
                         const NTL::vec_ZZ& z) const {
    // Validate dimensions first
    if (u.length() != params_.n()) {
        throw std::invalid_argument("Commitment vector has wrong dimension");
    }
    if (challenge.length() != params_.m()) {
        throw std::invalid_argument("Challenge vector has wrong dimension");
    }
    if (z.length() != params_.m()) {
        throw std::invalid_argument("Response vector has wrong dimension");
    }

    // Check norm bound
    long norm_bound = calculate_norm_bound(
        params_.m(), params_.y_range(), params_.s_range(), params_.safety_factor()
    );

    NTL::ZZ norm_sq = compute_norm_squared(z, params_.q());
    if (norm_sq > norm_bound) {
        std::cout << "Norm bound check failed (norm_bound was " << norm_bound << ")\n";
        return false;
    }

    // Compute Az
    NTL::vec_ZZ_p Az = matrix_vector_mod(A_, z);

    // Compute ct
    NTL::vec_ZZ_p ct;
    ct.SetLength(params_.n());
    for (int i = 0; i < params_.n(); i++) {
        ct[i] = NTL::conv<NTL::ZZ_p>(0);
        for (int j = 0; j < params_.m(); j++) {
            ct[i] += NTL::conv<NTL::ZZ_p>(challenge[j]) * A_[i][j] * NTL::conv<NTL::ZZ_p>(s_[j]);
        }
    }

    // Compute u + ct
    NTL::vec_ZZ_p rhs;
    rhs.SetLength(params_.n());
    for (int i = 0; i < params_.n(); i++) {
        rhs[i] = u[i] + ct[i];
    }

    return Az == rhs;
}

NTL::vec_ZZ LatticeProof::generate_challenge(int length) {
    return protocol::generate_challenge(length);
}

} // namespace protocol