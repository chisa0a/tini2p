/* Copyright (c) 2019, tini2p
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Parts used from the Kovri Project Copyright (c) 2013-2018
*/

#ifndef SRC_CRYPTO_KEY_ELGAMAL_H_
#define SRC_CRYPTO_KEY_ELGAMAL_H_

#include <cryptopp/elgamal.h>
#include <cryptopp/queue.h>
#include <cryptopp/osrng.h>

#include "src/exception/exception.h"

#include "src/crypto/constants.h"

namespace ntcp2
{
namespace meta
{
namespace crypto
{
namespace elgamal
{
enum
{
  BitSize = 2048,
};
}  // namespace elgamal
}  // namespace crypto
}  // namespace meta

namespace crypto
{
namespace pk
{
enum
{
  ElGamalLen = 256,
};

using ElGamal = CryptoPP::FixedSizeSecBlock<std::uint8_t, ElGamalLen>;
}  // namespace pk

namespace sk
{
enum
{
  ElGamalLen = 256,
};

using ElGamal = CryptoPP::FixedSizeSecBlock<std::uint8_t, ElGamalLen>;
}  // namespace sk

namespace elgamal
{
struct Keypair
{
  ntcp2::crypto::pk::ElGamal pk;
  ntcp2::crypto::sk::ElGamal sk;
};

inline Keypair create_keys()
{
  using ntcp2::meta::crypto::constants::elgp;
  using ntcp2::meta::crypto::constants::elgg;

  CryptoPP::AutoSeededRandomPool rng;

  // generate the private key
  CryptoPP::ElGamalKeys::PrivateKey key;
  key.Initialize(rng, elgp, elgg);

  // generate the public key
  crypto::pk::ElGamal pk;
  const auto& x = key.GetPrivateExponent();
  a_exp_b_mod_c(elgg, x, elgp).Encode(pk.data(), pk.size());

  crypto::sk::ElGamal sk;
  x.Encode(sk.data(), sk.size());

  return {pk, sk};
}
}  // namespace elgamal
}  // namespace crypto
}  // namespace ntcp2

#endif  // SRC_CRYPTO_KEY_ELGAMAL_H_
