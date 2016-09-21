//
// Created by shaofei on 9/20/16.
//

#include "../Zerocoin.h"
#include "SerialNumberSignatureOfKnowledgeV2.h"
#include "CommitmentSoK.h"

using namespace std;

namespace libzerocoin {

    // Dummies
    SerialNumberSignatureOfKnowledgeV2::SerialNumberSignatureOfKnowledgeV2(int a) {
        cout << "Dummy SoK " << a << " constructed" << endl;
    }

    bool SerialNumberSignatureOfKnowledgeV2::Verify() {
        cout << "Dummy SoK verified" << endl;
    }

    SerialNumberSignatureOfKnowledgeV2::SerialNumberSignatureOfKnowledgeV2(const Params *p,
                                                                           const ParamsSoK *pSoK,
                                                                           const PrivateCoin &coin,
                                                                           const CommitmentSoK &commitmentToCoin,
                                                                           string msg) : params(p), pSoK(pSoK) {

        cout << "Constructing SoK proof..." << endl;

        // Compute y
        cout << "Computing y" << endl;
        y = commitmentToCoin.commitmentValue;

        // Compute t
        cout << "Computing t" << endl;
        Bignum a = params->coinCommitmentGroup.g;           // Generator (for serial number) for coin's commitment
        Bignum S = coin.getSerialNumber();                  // Serial number of coin
        Bignum g = pSoK->discreteLogGroup.g;                // Generator for commitment of coin's commitment
        Bignum p_g = pSoK->discreteLogGroup.modulus;        // Modulus for commitment of coin's commitment
        Bignum q_g = pSoK->discreteLogGroup.groupOrder;     // Group order for commitment of coin's commitment
        Bignum p_ab = params->coinCommitmentGroup.modulus;  // Modulus for coin's commitment
        assert(p_ab == q_g);                                // Important step, p_ab and q_g must be the same
        Bignum v = Bignum::randBignum(q_g);                 // Random number [1 ... q_g]
        t = g.pow_mod(a.pow_mod(S, p_ab).mul_mod(v, q_g), p_g);

        // Compute C
        cout << "Computing C" << endl;
        Bignum C = extractChallenge(msg);

        // Compute r
        cout << "Computing r" << endl;
        Bignum b = params->coinCommitmentGroup.h;           // Generator (for random trapdoor) for coin's commitment
        r = (v - C.mul_mod(b.pow_mod(coin.getRandomness(), p_ab), p_ab)) % p_ab;

        cout << "SoK proof constructed!" << endl;
        cout << "SoK proof size: " << this->GetSerializeSize(0,0) << endl;
        cout << endl;

    }

    bool SerialNumberSignatureOfKnowledgeV2::Verify(const Bignum &coinSerialNumber,
                                                    const Bignum &valueOfCommitmentToCoin,
                                                    const string msg) const {
        cout << "Verifying SoK proof..." << endl;

        // Compute C
        cout << "Computing C" << endl;
        Bignum C = extractChallenge(msg);

        // Compute g^(r * a^S) * y^C
        cout << "Computing g^(r * a^S) * y^C" << endl;
        Bignum g = pSoK->discreteLogGroup.g;                // Generator for commitment of coin's commitment
        Bignum a = params->coinCommitmentGroup.g;           // Generator (for serial number) for coin's commitment
        Bignum S = coinSerialNumber;                        // Serial number of coin
        Bignum p_g = pSoK->discreteLogGroup.modulus;        // Modulus for commitment of coin's commitment
        Bignum q_g = pSoK->discreteLogGroup.groupOrder;     // Group order for commitment of coin's commitment
        Bignum p_ab = params->coinCommitmentGroup.modulus;  // Modulus for coin's commitment
        assert(p_ab == q_g);                                // Important step, p_ab and q_g must be the same
        Bignum test = g.pow_mod(a.pow_mod(S, p_ab).mul_mod(r, q_g), p_g).mul_mod(y.pow_mod(C, p_g), p_g);

        cout << "Verifying t = g^(r * a^S) * y^C" << endl;
        if(test == t) {
            cout << "SoK proof sucessfully verified!" << endl << endl;
            return true;
        } else {
            cout << "SoK proof failed to verify!" << endl << endl;
            return false;
        }

    }

    Bignum SerialNumberSignatureOfKnowledgeV2::extractChallenge(string msg) const {
        // Important step, y and t must be computed before challenge can be computed
        assert(y != 0 && t != 0);
        CHashWriter hasher(0,0);
        hasher << msg << y << t;
        uint256 hash = hasher.GetHash();
        // Right shift hash so that only challengeLength bits are used for C
        hash = hash >> (256 - pSoK->challengeLength);
        Bignum C;
        C.setuint256(hash);
        return C;
    }
}