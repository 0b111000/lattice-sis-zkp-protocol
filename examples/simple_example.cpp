#include "protocol/lattice_proof.hpp"
#include <iostream>

using namespace protocol;

int main() {
    try {
        // Initialize with high dimension parameters
        auto params = Parameters::HighSecurityParams();
        std::cout << params.toString();

        // Create proof system
        LatticeProof proof(params);

        // Prover generates commitment
        auto u = proof.commit();
        std::cout << "Commitment generated\n";

        // Verifier generates challenge
        auto challenge = LatticeProof::generate_challenge(params.m());
        std::cout << "Challenge generated\n";

        // Prover generates response
        auto z = proof.respond(challenge);
        std::cout << "Response generated\n";

        // Verifier checks proof
        bool valid = proof.verify(u, challenge, z);
        std::cout << "Verification result: " << (valid ? "Success" : "Failure") << "\n";

        return valid ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}