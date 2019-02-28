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
*/

#include <catch2/catch.hpp>

#include "src/crypto/x25519.h"

using tini2p::crypto::HmacSha256;
using tini2p::crypto::X25519;

struct X25519Fixture
{
  const X25519::pvtkey_t alice_pvtkey{{
      0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1,
      0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0,
      0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a,
  }};

  const X25519::pubkey_t alice_pubkey{{
      0x85, 0x20, 0xf0, 0x09, 0x89, 0x30, 0xa7, 0x54, 0x74, 0x8b, 0x7d,
      0xdc, 0xb4, 0x3e, 0xf7, 0x5a, 0x0d, 0xbf, 0x3a, 0x0d, 0x26, 0x38,
      0x1a, 0xf4, 0xeb, 0xa4, 0xa9, 0x8e, 0xaa, 0x9b, 0x4e, 0x6a,
  }};

  const X25519::pvtkey_t bob_pvtkey{{
      0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b, 0x79, 0xe1, 0x7f,
      0x8b, 0x83, 0x80, 0x0e, 0xe6, 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18,
      0xb6, 0xfd, 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb,
  }};

  const X25519::pubkey_t bob_pubkey{{
      0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4, 0xd3, 0x5b, 0x61,
      0xc2, 0xec, 0xe4, 0x35, 0x37, 0x3f, 0x83, 0x43, 0xc8, 0x5b, 0x78,
      0x67, 0x4d, 0xad, 0xfc, 0x7e, 0x14, 0x6f, 0x88, 0x2b, 0x4f,
  }};

  const X25519::shrkey_t exp_shrkey{{
      0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1, 0x72, 0x8e, 0x3b,
      0xf4, 0x80, 0x35, 0x0f, 0x25, 0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1,
      0x9e, 0x33, 0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42,
  }};
};

TEST_CASE_METHOD(X25519Fixture, "X25519 computes shared key (from RFC 7748)", "[dh_x25519]")
{
  using Catch::Equals;
  using vec = std::vector<std::uint8_t>;

  X25519::pubkey_t apk, bpk;
  X25519::pubkey_t nine{};
  nine[0] = 0x09;
  X25519::shrkey_t ashrk, bshrk;

  // AlicePubKey = DH(AlicePvtKey, 0x09 || zero[31])
  REQUIRE_NOTHROW(X25519::DH(apk, alice_pvtkey, nine));

  REQUIRE_THAT(
      vec(alice_pubkey.begin(), alice_pubkey.end()),
      Equals(vec(apk.begin(), apk.end())));

  // BobPubKey = DH(BobPvtKey, 0x09 || zero[31])
  REQUIRE_NOTHROW(
      X25519::DH(bpk, bob_pvtkey, nine));

  REQUIRE_THAT(
      vec(bob_pubkey.begin(), bob_pubkey.end()),
      Equals(vec(bpk.begin(), bpk.end())));

  // AliceShareKey = DH(AlicePvtKey, BobPubKey)
  REQUIRE_NOTHROW(X25519::DH(ashrk, alice_pvtkey, bob_pubkey));

  REQUIRE_THAT(
      vec(ashrk.begin(), ashrk.end()),
      Equals(vec(exp_shrkey.begin(), exp_shrkey.end())));

  // BobShareKey = DH(BobPvtKey, AlicePubKey)
  REQUIRE_NOTHROW(X25519::DH(bshrk, bob_pvtkey, alice_pubkey));

  REQUIRE_THAT(
      vec(bshrk.begin(), bshrk.end()),
      Equals(vec(exp_shrkey.begin(), exp_shrkey.end())));
}