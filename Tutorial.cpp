/**
* @file       tutorial.cpp
*
* @brief      Simple tutorial program to illustrate Zerocoin usage.
*
* @author     Ian Miers, Christina Garman and Matthew Green
* @date       June 2013
*
* @copyright  Copyright 2013 Ian Miers, Christina Garman and Matthew Green
* @license    This project is released under the MIT license.
**/

using namespace std;

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <curses.h>
#include <exception>
#include "Zerocoin.h"

#define COINS_TO_ACCUMULATE     5
#define DUMMY_TRANSACTION_HASH  0 // in real life these would be uint256 hashes
#define DUMMY_ACCUMULATOR_ID    0 // in real life these would be uint256 hashes

//
// We generated this for testing only. Don't use it in production!
//
#define TUTORIAL_TEST_MODULUS   "a8852ebf7c49f01cd196e35394f3b74dd86283a07f57e0a262928e7493d4a3961d93d93c90ea3369719641d626d28b9cddc6d9307b9aabdbffc40b6d6da2e329d079b4187ff784b2893d9f53e9ab913a04ff02668114695b07d8ce877c4c8cac1b12b9beff3c51294ebe349eca41c24cd32a6d09dd1579d3947e5c4dcc30b2090b0454edb98c6336e7571db09e0fdafbd68d8f0470223836e90666a5b143b73b9cd71547c917bf24c0efc86af2eba046ed781d9acb05c80f007ef5a0a5dfca23236f37e698e8728def12554bc80f294f71c040a88eff144d130b24211016a97ce0f5fe520f477e555c9997683d762aff8bd1402ae6938dd5c994780b1bf6aa7239e9d8101630ecfeaa730d2bbc97d39beb057f016db2e28bf12fab4989c0170c2593383fd04660b5229adcd8486ba78f6cc1b558bcd92f344100dff239a8c00dbc4c2825277f24bdd04475bcc9a8c39fd895eff97c1967e434effcb9bd394e0577f4cf98c30d9e6b54cd47d6e447dcf34d67e48e4421691dbe4a7d9bd503abb9"

//
// The following routine exercises most of the core functions of
// the library. We've commented it as well as possible to give
// you the flavor of how libzerocoin works.
//
// For details on Zerocoin integration, see the libzerocoin wiki
// at: https://github.com/Zerocoin/libzerocoin/wiki
//

/** By sf
 *
 * Returns ms passed
 * @param start start time
 * @return
 */

clock_t timeElapsed(clock_t start) {
	return (clock() - start) / (double) CLOCKS_PER_SEC * 1000;
}

bool
ZerocoinTutorial()
{
	// The following simple code illustrates the call flow for Zerocoin
	// applications. In a real currency network these operations would
	// be split between individual payers/payees, network nodes and miners.
	//
	// For each call we specify the participant who would use it.

	// Zerocoin uses exceptions (based on the runtime_error class)
	// to indicate all sorts of problems. Always remember to catch them!

	try {

		clock_t startTime;

		/********************************************************************/
		// What is it:      Parameter loading
		// Who does it:     ALL ZEROCOIN PARTICIPANTS
		// What it does:    Loads a trusted Zerocoin modulus "N" and
		//                  generates all associated parameters from it.
		//                  We use a hardcoded "N" that we generated using
		//                  the included 'paramgen' utility.
		/********************************************************************/

		// Load a test modulus from our hardcoded string (above)
		Bignum testModulus;
		testModulus.SetHex(std::string(TUTORIAL_TEST_MODULUS));

		// Set up the Zerocoin Params object
		startTime = clock();
		libzerocoin::Params* params = new libzerocoin::Params(testModulus);
		cout << "Successfully loaded parameters." << endl;
		cout << "	Size of parameters: " << params->GetSerializeSize(0,0) << endl;
		cout << "	Time used: " << timeElapsed(startTime) << endl;

		/********************************************************************/
		// What is it:      Coin generation
		// Who does it:     ZEROCOIN CLIENTS
		// What it does:    Generates a new 'zerocoin' coin using the
		//                  public parameters. Once generated, the client
		//                  will transmit the public portion of this coin
		//                  in a ZEROCOIN_MINT transaction. The inputs
		//                  to this transaction must add up to the zerocoin
		//                  denomination plus any transaction fees.
		/********************************************************************/

		// The following constructor does all the work of minting a brand
		// new zerocoin. It stores all the private values inside the
		// PrivateCoin object. This includes the coin secrets, which must be
		// stored in a secure location (wallet) at the client.
		startTime = clock();
		libzerocoin::PrivateCoin newCoin(params);
		cout << "Successfully minted a zerocoin." << endl;
		std::cout << "	Size of new coin: " << newCoin.GetSerializeSize(0,0) << endl;
		// Get a copy of the 'public' portion of the coin. You should
		// embed this into a Zerocoin 'MINT' transaction along with a series
		// of currency inputs totaling the assigned value of one zerocoin.
		libzerocoin::PublicCoin pubCoin = newCoin.getPublicCoin();
		std::cout << "	Size of public coin params: " << pubCoin.GetSerializeSize(0,0) << endl;
		cout << "	Time used: " << timeElapsed(startTime) << endl;

		// Serialize the public coin to a CDataStream object.
		CDataStream serializedCoin(SER_NETWORK, PROTOCOL_VERSION);
		serializedCoin << pubCoin;

		/********************************************************************/
		// What is it:      Coin verification
		// Who does it:     TRANSACTION VERIFIERS
		// What it does:    Verifies the structure of a zerocoin obtained from
		//                  a ZEROCOIN_MINT transaction. All coins must be
		//                  verified before you operate on them.
		//                  Note that this is only part of the transaction
		//                  verification process! The client must also check
		//                  that (1) the inputs to the transaction are valid
		//                  and add up to the value of one zerocoin, (2) that
		//                  this particular zerocoin has not been minted before.
		/********************************************************************/

		// Deserialize the public coin into a fresh object.
		libzerocoin::PublicCoin pubCoinNew(params, serializedCoin);

		// Now make sure the coin is valid.
		if (!pubCoinNew.validate()) {
			// If this returns false, don't accept the coin for any purpose!
			// Any ZEROCOIN_MINT with an invalid coin should NOT be
			// accepted as a valid transaction in the block chain.
			cout << "Error: coin is not valid!";
		}
		
		cout << "Deserialized and verified the coin." << endl;

		/********************************************************************/
		// What is it:      Accumulator computation
		// Who does it:     ZEROCOIN CLIENTS & TRANSACTION VERIFIERS
		// What it does:    Collects a number of PublicCoin values drawn from
		//                  the block chain and calculates an accumulator.
		//                  This accumulator is incrementally computable;
		//                  you can stop and serialize it at any point
		//                  then continue accumulating new transactions.
		//                  The accumulator is also order-independent, so
		//                  the same coins can be accumulated in any order
		//                  to give the same result.
		//                  WARNING: do not accumulate the same coin twice!
		/********************************************************************/

		cout << "Generating test coins..." << endl;
		std::vector<libzerocoin::PrivateCoin> testCoins;
		testCoins.reserve(COINS_TO_ACCUMULATE);
		for (uint32_t i = 0; i < COINS_TO_ACCUMULATE; i++) {
			testCoins.emplace_back(libzerocoin::PrivateCoin(params));
		}

		// Create an empty accumulator object
		libzerocoin::Accumulator accumulator(params);
		cout << "Size of empty accumulator: " << accumulator.GetSerializeSize(0,0) << endl;

		// Add several coins to it (we'll generate them here on the fly).
		for (uint32_t i = 0; i < COINS_TO_ACCUMULATE; i++) {
//			libzerocoin::PrivateCoin testCoin(params);
			accumulator += testCoins[i].getPublicCoin();
//			accumulator += testCoin.getPublicCoin();
		}

		// Serialize the accumulator object.
		//
		// If you're using Accumulator Checkpoints, each miner would
		// start by deserializing the accumulator checkpoint from the
		// previous block (or creating a new Accumulator if no previous
		// block exists). It will then add all the coins in the new block,
		// then serialize the resulting Accumulator object to obtain the
		// new checkpoint. All block verifiers should do the same thing
		// to check their work.
		CDataStream serializedAccumulator(SER_NETWORK, PROTOCOL_VERSION);
		serializedAccumulator << accumulator;

		// Deserialize the accumulator object
		libzerocoin::Accumulator newAccumulator(params, serializedAccumulator);

		// We can now continue accumulating things into the accumulator
		// we just deserialized. For example, let's put in the coin
		// we generated up above.
		startTime = clock();
		newAccumulator += pubCoinNew;
		cout << "Successfully accumulated coins." << endl;
		cout << "	Size of accumulate accumulator: " << newAccumulator.GetSerializeSize(0,0) << endl;
		cout << "	Time used to accumulate 1 coin: " << timeElapsed(startTime) << endl;


		/********************************************************************/
		// What is it:      Coin spend
		// Who does it:     ZEROCOIN CLIENTS
		// What it does:    Create a new transaction that spends a Zerocoin.
		//                  The user first authors a transaction specifying
		//                  a set of destination addresses (outputs). They
		//                  next compute an Accumulator over all coins in the
		//                  block chain (see above) and a Witness based on the
		//                  coin to be spent. Finally they instantiate a CoinSpend
		//                  object that 'signs' the transaction with a special
		//                  zero knowledge signature of knowledge over the coin
		//                  data, Witness and Accumulator.
		/********************************************************************/

		// We are going to spend the coin "newCoin" that we generated at the
		// top of this function.
		//
		// We'll use the accumulator we constructed above. This contains
		// a set of coins, but does NOT include the coin "newCoin".
		//
		// To generate the witness, we start with this accumulator and
		// add the public half of the coin we want to spend.
		startTime = clock();
		libzerocoin::AccumulatorWitness witness(params, accumulator, newCoin.getPublicCoin());

		// Add the public half of "newCoin" to the Accumulator itself.
		accumulator += newCoin.getPublicCoin();

		// At this point we should generate a ZEROCOIN_SPEND transaction to
		// send to the network. This network should include a set of outputs
		// totalling to the value of one zerocoin (minus transaction fees).
		//
		// The format of this transaction is up to the implementer. Here we'll
		// assume you've formatted this transaction and placed the hash into
		// "transactionHash". We'll also assume "accumulatorHash" contains the
		// hash of the last block whose transactions are in the accumulator.
		uint256 transactionHash = DUMMY_TRANSACTION_HASH;
		uint256 accumulatorID = DUMMY_ACCUMULATOR_ID;

		// Place "transactionHash" and "accumulatorBlockHash" into a new
		// SpendMetaData object.
		libzerocoin::SpendMetaData metaData(accumulatorID, transactionHash);

		// Construct the CoinSpend object. This acts like a signature on the
		// transaction.
		libzerocoin::CoinSpend spend(params, newCoin, accumulator, witness, metaData);

		cout << "Successfully generated a coin spend transaction." << endl;
		cout << "	Size of spend transaction: " << spend.GetSerializeSize(0,0) << endl;
		cout << "	Time used: " << timeElapsed(startTime) << endl;

		// This is a sanity check. The CoinSpend object should always verify,
		// but why not check before we put it onto the wire?
		if (!spend.Verify(accumulator, metaData)) {
			cout << "ERROR: Our new CoinSpend transaction did not verify!" << endl;
			return false;
		}

		// Serialize the CoinSpend object into a buffer.
		CDataStream serializedCoinSpend(SER_NETWORK, PROTOCOL_VERSION);
		// Serialize the CoinSpend object into file
//		FILE * spendTxFile = fopen ("/home/shaofei/Desktop/myfile.txt" , "w");

//		CAutoFile serializedCoinSpend(spendTxFile, 0,0);
		serializedCoinSpend << spend;
//		fclose(spendTxFile);

		/********************************************************************/
		// What is it:      Coin spend verification
		// Who does it:     ALL PARTIES
		// What it does:    Verifies that a CoinSpend signature is correct
		//                  with respect to a ZEROCOIN_SPEND transaction hash.
		//                  The client must also extract the serial number from
		//                  the CoinSpend and verify that this serial number has
		//                  not previously appeared in another ZEROCOIN_SPEND
		//                  transaction.
		/********************************************************************/

//		FILE * spendTxFileReceived = fopen ("/home/shaofei/Desktop/myfile.txt" , "r");
//		CAutoFile serializedCoinSpendReceived(spendTxFileReceived, 0,0);
//		fclose(spendTxFileReceived);

		// Deserialize the CoinSpend intro a fresh object
		libzerocoin::CoinSpend newSpend(params, serializedCoinSpend);
//		libzerocoin::CoinSpend newSpend(params, serializedCoinSpendReceived);


		// Create a new metadata object to contain the hash of the received
		// ZEROCOIN_SPEND transaction. If we were a real client we'd actually
		// compute the hash of the received transaction here.
		libzerocoin::SpendMetaData newMetadata(accumulatorID, transactionHash);
		
		// If we were a real client we would now re-compute the Accumulator
		// from the information given in the ZEROCOIN_SPEND transaction.
		// For our purposes we'll just use the one we calculated above.
		//
		// Verify that the spend is valid with respect to the Accumulator
		// and the Metadata
		startTime = clock();
		if (!newSpend.Verify(accumulator, newMetadata)) {
			cout << "ERROR: The CoinSpend transaction did not verify!" << endl;
			return false;
		}

		cout << "Successfully verified a coin spend transaction." << endl;
		cout << "	Time used: " << timeElapsed(startTime) << endl;

		// Pull the serial number out of the CoinSpend object. If we
		// were a real Zerocoin client we would now check that the serial number
		// has not been spent before (in another ZEROCOIN_SPEND) transaction.
		// The serial number is stored as a Bignum.
		Bignum serialNumber = newSpend.getCoinSerialNumber();
		cout << endl << "Coin serial number is:" << endl << serialNumber << endl;

		// We're done
		return true;

	} catch (runtime_error &e) {
		cout << e.what() << endl;
		return false;
	}

	return false;
}

int main(int argc, char **argv)
{
	cout << "libzerocoin v" << ZEROCOIN_VERSION_STRING << " tutorial." << endl << endl;

	ZerocoinTutorial();
}
