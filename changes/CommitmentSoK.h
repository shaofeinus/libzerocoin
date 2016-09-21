//
// Created by shaofei on 9/21/16.
//

#ifndef ZEROCOIN_COMMITMENTSOK_H
#define ZEROCOIN_COMMITMENTSOK_H

#include "ParamsSoK.h"
#include "../bitcoin_bignum/serialize.h"

namespace libzerocoin {
    /**
     * @brief A discrete log commitment
     */
    class CommitmentSoK {
    public:
        CommitmentSoK(const ParamsSoK* params, const Bignum& contents);
        Bignum commitmentValue;
        const ParamsSoK* params;
        IMPLEMENT_SERIALIZE
        (
                READWRITE(commitmentValue);
        )
    };
}


#endif //ZEROCOIN_COMMITMENTSOK_H
