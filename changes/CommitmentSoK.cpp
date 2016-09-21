//
// Created by shaofei on 9/21/16.
//

#include "CommitmentSoK.h"
#include "../Zerocoin.h"

using namespace std;

namespace libzerocoin {

    CommitmentSoK::CommitmentSoK(const ParamsSoK *params, const Bignum &contents) : params(params) {
        cout << "Creating SoK commitment..." << endl;
        // (g^contents) mod p
        this->commitmentValue = params->discreteLogGroup.g.pow_mod(contents, params->discreteLogGroup.modulus);
        cout << "SoK commitment created!" << endl;
        cout << "Commitment value: " << this->commitmentValue.GetHex() << endl << endl;
    }


}

