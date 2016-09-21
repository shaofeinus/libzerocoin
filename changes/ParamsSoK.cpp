//
// Created by shaofei on 9/21/16.
//

#include "../Zerocoin.h"
#include "ParamsSoK.h"

using namespace std;

namespace libzerocoin {
    ParamsSoK::ParamsSoK(Bignum coinCommitmentModulus, uint32_t securityLevel) : challengeLength(securityLevel) {
        cout << "Initializing SoK params..." << endl;
        /**
         * Calculate the DiscreteLogGroup params using functions provided in ParamGeneration.cpp
         */

        this->discreteLogGroup = deriveIntegerGroupFromOrder(coinCommitmentModulus);
        this->initialized = true;
        cout << "SoK params initialized!" << endl;
        cout << "SoK group modulus: " << this->discreteLogGroup.modulus.GetHex() << endl
             << "SoK group order: " << this->discreteLogGroup.groupOrder.GetHex() << endl << endl;
    }
}
