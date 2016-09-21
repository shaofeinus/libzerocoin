//
// Created by shaofei on 9/21/16.
//

#ifndef ZEROCOIN_PARAMSSOK_H
#define ZEROCOIN_PARAMSSOK_H

#include "../Zerocoin.h"
#include "../bitcoin_bignum/bignum.h"
#include "../bitcoin_bignum/serialize.h"

namespace libzerocoin {

    /**
     * @brief This class contains the parameters required for the SoK proof
     */
    class ParamsSoK {

    public:
        /**
         * @brief This is a Schnorr group with the specific property that the order of the group
         * must be exactly equal to modulus "p" from the coin commitment group. We set
         * the modulus of the new group equal to "2q+1" and test to see if this is prime.
         * @param coinCommitmentModulus modulus of the coin commitment group
         * @param securityLevel length of challenge string
         * @return
         */
        ParamsSoK(Bignum coinCommitmentModulus, uint32_t securityLevel = ZEROCOIN_DEFAULT_SECURITYLEVEL);

        bool initialized;
//        DiscreteLogGroup discreteLogGroup;
        IntegerGroupParams discreteLogGroup;

        /**
         * The length of c used as the challenge string
         */
        uint32_t challengeLength;

        IMPLEMENT_SERIALIZE
        (
                READWRITE(initialized);
                READWRITE(discreteLogGroup);
                READWRITE(challengeLength);
        )
    };

    /**
     * @brief This class contains the paramters required for the discrete log proof
     */
    class DiscreteLogGroup {

    public:
        DiscreteLogGroup();

        bool initialized;

        /**
         * The generator for the group
         */
        Bignum g;

        /**
         * The modulus for the group, a large prime.
         */
        Bignum p_modulus;

        /**
         * The order of the group, satisfies q | p - 1
         */
        Bignum q_order;

        IMPLEMENT_SERIALIZE
        (
                READWRITE(initialized);
                READWRITE(g);
                READWRITE(p_modulus);
                READWRITE(q_order);
        )

    };
}


#endif //ZEROCOIN_PARAMSSOK_H
