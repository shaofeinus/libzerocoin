//
// Created by shaofei on 9/20/16.
//

#ifndef ZEROCOIN_SERIALNUMBERSIGNATUREOFKNOWLEDGEV2_H
#define ZEROCOIN_SERIALNUMBERSIGNATUREOFKNOWLEDGEV2_H

#include "../Params.h"
#include "../bitcoin_bignum/bignum.h"
#include "../bitcoin_bignum/serialize.h"
#include "ParamsSoK.h"
#include "CommitmentSoK.h"

namespace libzerocoin {
    /**
     * @brief A Signature of Knowledge on the hash of metadata attesting that the signer knows
     * 1. The values needed to open the commitment to the commitment of the coin that he owns
     * 2. The serial number S and the random trapdoor r needed to open a commitment to the coin he owns
     */
    class SerialNumberSignatureOfKnowledgeV2 {
    public:

        // Dummies
        SerialNumberSignatureOfKnowledgeV2(int a);
        bool Verify();

        /**
         * @brief This constructor is used to convert a stream to the object
         * @param p as per actual constructor
         * @param paramsSoK as per actual constructor
         * @return
         */
        template<typename Stream>
        SerialNumberSignatureOfKnowledgeV2(const Params* p, const ParamsSoK* pSoK, Stream& strm) : params(p), pSoK(pSoK) {
            strm >> *this;
        }

        /**
         *
         * @brief This constructor constructs the proof for the SoK for a given coin.
         *
         * @param p contains the generator, modulus and group order used in the coin commitment
         * @param pSoK contains the generator, modulus and group order used in the discrete log proof
         * @param coin contains the serial number S and the random trapdoor r for the coin
         * @param commitmentToCoin the commitment value of the coin
         * @param msghash hash of message (metadata)
         * @return
         */
        SerialNumberSignatureOfKnowledgeV2(const Params *p, const ParamsSoK *pSoK, const PrivateCoin &coin, const CommitmentSoK &commitmentToCoin,
                                         string msg);

        /**
         *
         * @brief This verifies that the proof constructed in the constructor for the given the inputs from the prover
         *
         * @param coinSerialNumber S of the commitment to coin
         * @param valueOfCommitmentToCoin commitment to the commitment of the coins
         * @param msghash hash of message (metadata)
         * @return true if proof is verified, false otherwise
         */
        bool Verify(const Bignum &coinSerialNumber, const Bignum &valueOfCommitmentToCoin, const string msg) const;

        IMPLEMENT_SERIALIZE
        (
                READWRITE(y);
                READWRITE(t);
                READWRITE(r);
        )

    private:
        const Params *params;
        const ParamsSoK *pSoK;
        Bignum y;       // Commitment of coin's commitment = g^c = g^(a^S * b^z)
        Bignum t;       // Random commitment = g^(a^S * v)
        Bignum r;       // Witness = v - Cb^z, hiding the trapdoor to coin's commitment, z

        /**
         * @brief Extracts the random challenge from y, t and message to be signed
         * @param msg message to be signed
         * @return random challenge C
         */
        Bignum extractChallenge(string msg) const;
    };
}


#endif //ZEROCOIN_SERIALNUMBERSIGNATUREOFKNOWLEDGEV2_H
