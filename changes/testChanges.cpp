//
// Created by shaofei on 9/20/16.
//

#include "../Zerocoin.h"
#include "SerialNumberSignatureOfKnowledgeV2.h"
#include "CommitmentSoK.h"

using namespace std;

#define TUTORIAL_TEST_MODULUS "a8852ebf7c49f01cd196e35394f3b74dd86283a07f57e0a262928e7493d4a3961d93d93c90ea3369719641d626d28b9cddc6d9307b9aabdbffc40b6d6da2e329d079b4187ff784b2893d9f53e9ab913a04ff02668114695b07d8ce877c4c8cac1b12b9beff3c51294ebe349eca41c24cd32a6d09dd1579d3947e5c4dcc30b2090b0454edb98c6336e7571db09e0fdafbd68d8f0470223836e90666a5b143b73b9cd71547c917bf24c0efc86af2eba046ed781d9acb05c80f007ef5a0a5dfca23236f37e698e8728def12554bc80f294f71c040a88eff144d130b24211016a97ce0f5fe520f477e555c9997683d762aff8bd1402ae6938dd5c994780b1bf6aa7239e9d8101630ecfeaa730d2bbc97d39beb057f016db2e28bf12fab4989c0170c2593383fd04660b5229adcd8486ba78f6cc1b558bcd92f344100dff239a8c00dbc4c2825277f24bdd04475bcc9a8c39fd895eff97c1967e434effcb9bd394e0577f4cf98c30d9e6b54cd47d6e447dcf34d67e48e4421691dbe4a7d9bd503abb9"
#define RANDOM_MSG "Hello world!"

void runSerialNumberSoK() {

    // Load a test modulus from our hardcoded string (above) used to set up the Zerocoin Params object
    Bignum testModulus;
    testModulus.SetHex(std::string(TUTORIAL_TEST_MODULUS));

    cout << "A side" << endl;
    // Set up params
    libzerocoin::Params* params = new libzerocoin::Params(testModulus);
    // Set up the params of the new SoK scheme
    libzerocoin::ParamsSoK* paramsSoK = new libzerocoin::ParamsSoK(params->coinCommitmentGroup.modulus);
    // Create new coin
    libzerocoin::PrivateCoin newCoin(params);
    libzerocoin::PublicCoin pubCoin = newCoin.getPublicCoin();
    // Create commitment to coin's commitment
    libzerocoin::CommitmentSoK SoKCommitment(paramsSoK, pubCoin.getValue());
    // Construct a new SoK proof
    libzerocoin::SerialNumberSignatureOfKnowledgeV2 SoK(params, paramsSoK, newCoin, SoKCommitment, RANDOM_MSG);     // Dummy
    // Send the proof
    cout << "Sending proof..." << endl;
    CDataStream serializedSoK(SER_NETWORK, PROTOCOL_VERSION);
    serializedSoK << SoK;
    cout << "Proof sent!" << endl << endl;

    cout << "B side" << endl;
    // Set up params afresh, to simulate independent params between nodes
    libzerocoin::Params* params2 = new libzerocoin::Params(testModulus);
    libzerocoin::ParamsSoK* paramsSoK2 = new libzerocoin::ParamsSoK(params2->coinCommitmentGroup.modulus);
    // Receive the proof, and supply proof with the params generated at this side (B)
    cout << "Receiving proof..." << endl;
    libzerocoin::SerialNumberSignatureOfKnowledgeV2 SoKReceived(params2, paramsSoK2, serializedSoK);
    cout << "Proof received!" << endl << endl;
    // Verify SoK proof
    SoKReceived.Verify(newCoin.getSerialNumber(), SoKCommitment.commitmentValue, RANDOM_MSG);
}


int main() {
    runSerialNumberSoK();
}