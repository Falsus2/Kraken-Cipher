# Kraken Cipher

A custom symmetric stream cipher built from scratch in C++, using a chain of
established cryptographic hash functions (rather than a traditional block
cipher) to generate a pseudorandom keystream.

> **⚠️ Status: experimental / learning project.** This was built to explore
> cipher design, not as a vetted cryptographic primitive. It has not been
> reviewed by cryptographers, has no formal security proof, and should not be
> used to protect real, sensitive data. See [Security Notes](#security-notes)
> below.

## How it works

Kraken Cipher is a **stream cipher**: it generates a pseudorandom keystream
from a secret key and a per-message nonce, then encrypts by XOR-ing that
keystream against the plaintext. Decryption runs the identical process and
XORs again, since `(P ⊕ K) ⊕ K = P`.

### Key and nonce

- **Private key:** 256 bits, supplied as a 64-character hex string, known
  only to the sender and receiver.
- **Nonce:** 64 bits, generated fresh and randomly for every message using a
  cryptographically secure random number generator (CryptoPP's
  `AutoSeededRandomPool`). The nonce is not secret — it's appended to the
  ciphertext in the clear so the receiver can use it during decryption.

Using a new random nonce per message means the same plaintext encrypted
twice under the same key produces two completely different ciphertexts.

### Keystream generation

The keystream is produced in 64-byte blocks, generated one at a time until
there's enough keystream to cover the whole message. This runs in **CTR
(counter) mode**: each block's input is just the nonce and an incrementing
counter, with no dependency on any other block's output. Each block goes
through three hash stages:

```
block input  = nonce (8 bytes) + counter (4 bytes)
round 1      = BLAKE3-MAC(key, block input)
round 2      = Whirlpool(round 1)
block output = SHA3-512(round 2)
```

- **Counter:** starts at 0 and increments once per block, so every block in
  a message gets a distinct input even though they share the same nonce.
- **No block chaining:** blocks are computed independently of each other —
  block *N*'s input depends only on the nonce and the value *N*, not on any
  previous block's output. This means blocks can, in principle, be computed
  in any order or in parallel, which is the defining property of CTR mode.
- **Three-stage hashing:** running the input through three different hash
  constructions (BLAKE3-MAC, then Whirlpool, then SHA3-512) rather than a
  single hash is intended to make the relationship between the input and the
  final output harder to analyze than any one algorithm alone.

The resulting 64-byte blocks are concatenated until the keystream is at
least as long as the plaintext, then truncated to match.

### Encryption

```
ciphertext = plaintext XOR keystream
output     = hex(ciphertext) + hex(nonce)
```

The final output is a single hex string: the encrypted bytes, followed by
the 16-hex-character (8-byte) nonce used to generate that keystream.

### Decryption

```
nonce      = last 16 hex characters of the input
ciphertext = everything before that
keystream  = KeyStreamGenerator(key, ciphertext length, nonce)
plaintext  = ciphertext XOR keystream
```

Since decryption regenerates the exact same keystream from the same key and
the nonce recovered from the ciphertext, XOR-ing again recovers the original
plaintext.

## Usage

```
Input your private key (256 bit hex):
<64 hex characters>

Would you like to encrypt or decrypt? (E or D)
E

Please input your message:
<your message>
```

The program prints the ciphertext as a hex string with the nonce appended.
Feed that same string back in with `D` (decrypt mode) and the same key to
recover the original message.

## Building

Built with C++ and the [cryptopp-modern](https://cryptopp-modern.com) library. See `CMakeLists.txt` for the exact dependency setup.

## Security Notes

A few things worth knowing if you're evaluating this project:

- **Not independently reviewed.** Real cryptographic primitives earn trust
  through years of public cryptanalysis. This hasn't had any.
- **Nonce size:** a 64-bit random nonce gives a comfortable collision margin
  for realistic message volumes per key (birthday bound ≈ 2^32 messages
  before meaningful collision risk), but is smaller than the 96–128 bit
  nonces typical of production AEAD schemes.
- **No authentication.** This cipher provides confidentiality only — it does
  not verify that ciphertext hasn't been tampered with in transit. A
  production system would pair this with a MAC (e.g. HMAC over the
  ciphertext) to detect modification.
- **Composing multiple hash functions doesn't automatically add security.**
  Stacking BLAKE3-MAC, Whirlpool, and SHA3-512 is an interesting design
  experiment, but whether this specific composition is actually stronger
  than any one of them alone is an open question this project doesn't
  answer.

This project is best understood as a working demonstration of stream cipher
mechanics — nonces, keystreams, XOR encryption, block chaining — built from
primitives rather than a cipher ready for real-world use.
