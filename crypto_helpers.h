#ifndef CRYPTO_HELPERS_H
#define CRYPTO_HELPERS_H

#include <string>
#include <cryptopp/secblock.h>

using namespace CryptoPP;
using namespace std;

SecByteBlock generateNonce();

SecByteBlock HexToBytes(const string& input);

string BytesToHex(const SecByteBlock& byteString);

SecByteBlock BLAKE3_MAC(const SecByteBlock& key, const SecByteBlock& input);

SecByteBlock WhirlpoolHash(const SecByteBlock& input);

SecByteBlock SHA3_512Hash(const SecByteBlock& input);

SecByteBlock KeyStreamGenerator(const string& key, const size_t& length, const SecByteBlock& nonce);

void Encrypt(const string& key, const string& plaintext);

void Decrypt(const string& key, const string& ciphertext);

#endif