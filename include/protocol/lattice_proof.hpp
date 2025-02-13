#pragma once

#include "parameters.hpp"
#include "utils.hpp"
#include <NTL/mat_ZZ_p.h>
#include <NTL/vec_ZZ_p.h>

namespace protocol {

class LatticeProof {
public:
    explicit LatticeProof(const Parameters& params);
    
    // Protocol operations
    NTL::vec_ZZ_p commit();
    NTL::vec_ZZ respond(const NTL::vec_ZZ& challenge);
    bool verify(const NTL::vec_ZZ_p& u, 
               const NTL::vec_ZZ& challenge, 
               const NTL::vec_ZZ& z) const;
    
    // Getters
    NTL::mat_ZZ_p getA() const { return A_; }
    NTL::vec_ZZ_p getT() const { return t_; }
    
    // Static methods
    static NTL::vec_ZZ generate_challenge(int length);

private:
    const Parameters& params_;
    NTL::mat_ZZ_p A_;  // Public matrix
    NTL::vec_ZZ s_;    // Secret vector
    NTL::vec_ZZ y_;    // Random vector for commitment
    NTL::vec_ZZ_p t_;  // Public value (As)
};

} // namespace protocol