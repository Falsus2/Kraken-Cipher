#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/blake3.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/sha3.h>
#include <cryptopp/secblock.h>
#include "crypto_helpers.h"
#include <iostream>

using namespace CryptoPP;
using namespace std;

SecByteBlock generateNonce() {
    AutoSeededRandomPool rng;
    SecByteBlock output(8);

    rng.GenerateBlock(output, output.size());

    return output;
}

SecByteBlock HexToBytes(const string& input) {
    SecByteBlock output(input.size() / 2);

    StringSource(
        input,
        true,
        new HexDecoder(
            new ArraySink(output.data(), output.size())
        )
    );

    return output;
}

string BytesToHex(const SecByteBlock& byteString) {
    string output;

    ArraySource(
        byteString,
        byteString.size(),
        true,
        new HexEncoder(
            new StringSink(output)
        )
    );

    return output;
}

SecByteBlock BLAKE3_MAC(const SecByteBlock& key, const SecByteBlock& input) {
    SecByteBlock output(BLAKE3::DIGESTSIZE);

    BLAKE3 mac(key, key.size());

    ArraySource(
        input,
        input.size(),
        true,
        new HashFilter(
            mac,
            new ArraySink(output.data(), output.size())
        )
    );

    return output;
}

SecByteBlock WhirlpoolHash(const SecByteBlock& input) {
    SecByteBlock output(Whirlpool::DIGESTSIZE);
    Whirlpool hash;

    ArraySource(
        input,
        input.size(),
        true,
        new HashFilter(
            hash,
            new ArraySink(output.data(), output.size())
        )
    );

    return output;
}

SecByteBlock SHA3_512Hash(const SecByteBlock& input) {
    SecByteBlock output(SHA3_512::DIGESTSIZE);
    SHA3_512 hash;

    ArraySource(
        input,
        input.size(),
        true,
        new HashFilter(
            hash,
            new ArraySink(output.data(), output.size())
        )
    );

    return output;
}

SecByteBlock KeyStreamGenerator(const string& key, const size_t& length, const SecByteBlock& nonce) {
    const SecByteBlock keyBytes = HexToBytes(key);
    SecByteBlock output;

    uint32_t counter = 0;

    for (int i = 0; i < length; i += 64) {
        const SecByteBlock counterBytes(reinterpret_cast<byte*>(&counter), sizeof(counter));

        SecByteBlock inputBytes;
        inputBytes += nonce;
        inputBytes += counterBytes;
        // Concatenate nonce with counter

        SecByteBlock round1 = BLAKE3_MAC(keyBytes, inputBytes);
        SecByteBlock round2 = WhirlpoolHash(round1);
        output += SHA3_512Hash(round2);

        counter++;
    }

    return output;
}

void Encrypt(const string& key, const string& plaintext) {
    const SecByteBlock nonce = generateNonce();
    const SecByteBlock keyStream = KeyStreamGenerator(key, plaintext.size(), nonce);

    SecByteBlock output(plaintext.size());

    for (int i = 0; i < plaintext.size(); i++) {
        output[i] = plaintext[i] ^ keyStream[i];
    }

    cout << "Ciphertext: " << BytesToHex(output) << BytesToHex(nonce) << endl;
}

void Decrypt(const string& key, const string& ciphertext) {
    const SecByteBlock cipherBytes = HexToBytes(ciphertext.substr(0, ciphertext.size() - 16));
    const SecByteBlock nonceBytes = HexToBytes(ciphertext.substr(ciphertext.size() - 16));

    SecByteBlock keyStream = KeyStreamGenerator(key, cipherBytes.size(), nonceBytes);

    string output(cipherBytes.size(), '\0');

    for (int i = 0; i < cipherBytes.size(); i++) {
        output[i] = static_cast<char>(cipherBytes[i] ^ keyStream[i]);
    }

    cout << "Decrypted message: " << output << endl;
}

