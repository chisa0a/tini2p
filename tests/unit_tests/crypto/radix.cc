/**                                                                                           //
 * Copyright (c) 2015-2018, The Kovri I2P Router Project                                      //
 * Copyright (c) 2019, tini2p
 *                                                                                            //
 * All rights reserved.                                                                       //
 *                                                                                            //
 * Redistribution and use in source and binary forms, with or without modification, are       //
 * permitted provided that the following conditions are met:                                  //
 *                                                                                            //
 * 1. Redistributions of source code must retain the above copyright notice, this list of     //
 *    conditions and the following disclaimer.                                                //
 *                                                                                            //
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list     //
 *    of conditions and the following disclaimer in the documentation and/or other            //
 *    materials provided with the distribution.                                               //
 *                                                                                            //
 * 3. Neither the name of the copyright holder nor the names of its contributors may be       //
 *    used to endorse or promote products derived from this software without specific         //
 *    prior written permission.                                                               //
 *                                                                                            //
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY        //
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF    //
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL     //
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       //
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,               //
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    //
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,          //
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF    //
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               //
 */

#include <catch2/catch.hpp>

#include "src/crypto/radix.h"

struct RadixFixture
{
  /// @brief Destination hash
  const std::vector<std::uint8_t> dest_hash{
      0x96, 0xba, 0x2f, 0xf4, 0x1c, 0xa3, 0xd2, 0x10, 0x8f, 0xc6, 0x2e,
      0xa1, 0xbe, 0x33, 0x99, 0xb2, 0xa5, 0x7e, 0xe4, 0xcd, 0xca, 0x65,
      0x3e, 0x47, 0x2e, 0x73, 0xaf, 0x78, 0xe0, 0xae, 0xe5, 0x25};

  /// @brief Destination
  const std::vector<std::uint8_t> dest{
      0xf1, 0xf6, 0xde, 0xc9, 0x18, 0xed, 0x51, 0xf0, 0xd4, 0xbb, 0x2a, 0xfd,
      0x7f, 0x96, 0x52, 0xcb, 0xf7, 0x1d, 0xfb, 0x71, 0x8c, 0x2d, 0x0c, 0x46,
      0xfa, 0x8e, 0xfe, 0x39, 0xe0, 0x46, 0x64, 0x89, 0xaf, 0x4b, 0xe1, 0x36,
      0xb1, 0xdb, 0xa7, 0xd6, 0x64, 0xa8, 0x2e, 0xfb, 0x9b, 0xb9, 0xf3, 0x50,
      0x8a, 0x0e, 0x09, 0xfa, 0x6b, 0x42, 0xde, 0xa6, 0x33, 0x6c, 0x4d, 0xcd,
      0x20, 0x44, 0xae, 0xb9, 0xcf, 0x85, 0xad, 0xf8, 0xce, 0xeb, 0xd9, 0xca,
      0x17, 0x46, 0x3b, 0x42, 0xfc, 0x52, 0x27, 0xe7, 0x7c, 0x12, 0xd0, 0xb9,
      0xf5, 0x40, 0x0e, 0x8b, 0x1f, 0xb5, 0x41, 0x23, 0xbc, 0xa0, 0xfd, 0xd0,
      0x03, 0xdd, 0x78, 0xb3, 0xf2, 0x48, 0x98, 0x0c, 0xfc, 0x31, 0x91, 0x0f,
      0x6c, 0xae, 0x3d, 0x21, 0x62, 0x5e, 0xb2, 0xf0, 0x7d, 0x77, 0x1b, 0x05,
      0x14, 0xe0, 0x3b, 0x8a, 0x89, 0x99, 0x02, 0x57, 0xed, 0xc3, 0xdb, 0x8d,
      0x13, 0x5f, 0x24, 0x80, 0x17, 0x46, 0x47, 0x0d, 0xfa, 0xfb, 0x7b, 0x5f,
      0x41, 0xd5, 0x4b, 0xf1, 0x00, 0x64, 0x5f, 0xcd, 0xa1, 0x63, 0x2f, 0xeb,
      0x90, 0x86, 0x00, 0x4c, 0xbd, 0x59, 0xa2, 0xaf, 0xf5, 0xe6, 0xa8, 0x86,
      0x50, 0x52, 0xc4, 0x1b, 0x84, 0xdc, 0x10, 0x19, 0xa6, 0xd6, 0xcf, 0xfb,
      0xa9, 0x2d, 0x09, 0x24, 0xb3, 0x7e, 0x53, 0x05, 0x8a, 0x4f, 0x91, 0x84,
      0x05, 0x99, 0xb8, 0x10, 0x67, 0x08, 0x72, 0x88, 0xc0, 0x43, 0xf3, 0xb3,
      0xdd, 0xef, 0x0a, 0x63, 0x62, 0x15, 0xf8, 0xdc, 0x83, 0xd1, 0x66, 0xb8,
      0x8e, 0x2b, 0xb2, 0xdb, 0xc5, 0x0b, 0x77, 0xaa, 0xaa, 0xa5, 0x95, 0x40,
      0x3c, 0xdc, 0x1f, 0xf1, 0x13, 0x5e, 0xca, 0xc0, 0xc0, 0x79, 0x7e, 0x3f,
      0x09, 0xa3, 0xe9, 0x64, 0x54, 0xd8, 0x28, 0x92, 0x8d, 0xff, 0xaa, 0x85,
      0xa4, 0xed, 0xf1, 0x2e, 0xd7, 0x28, 0x27, 0x5d, 0xab, 0x31, 0xf1, 0xba,
      0xb8, 0xcc, 0x20, 0x3b, 0xeb, 0x78, 0x42, 0xc6, 0x2f, 0x98, 0x2b, 0x7a,
      0x31, 0x31, 0xff, 0x90, 0xbe, 0x20, 0xcf, 0xe9, 0x88, 0x4a, 0xfe, 0x69,
      0x08, 0x6c, 0xcf, 0xf7, 0x4e, 0xc8, 0x87, 0x80, 0xc8, 0xfe, 0x99, 0x2a,
      0x17, 0xdd, 0x2b, 0x3f, 0x48, 0x21, 0xff, 0x75, 0x48, 0x70, 0xc7, 0xc0,
      0x8b, 0x53, 0x7c, 0x14, 0x6b, 0x19, 0x5e, 0x7f, 0x02, 0x75, 0x5a, 0xf9,
      0x72, 0x3a, 0x88, 0x0c, 0xd3, 0xe6, 0x35, 0xba, 0x71, 0x9e, 0x1d, 0xa2,
      0x55, 0x44, 0x43, 0x13, 0x9c, 0xaa, 0x6c, 0xec, 0xea, 0x4e, 0xc7, 0x82,
      0x6e, 0x4d, 0xb2, 0x6f, 0x3b, 0xdc, 0x0a, 0x14, 0xe5, 0x62, 0xe8, 0xcc,
      0x48, 0x43, 0xb5, 0x36, 0xfc, 0x6c, 0xf1, 0x55, 0xf7, 0xa7, 0x2a, 0xea,
      0xed, 0xb7, 0x05, 0x0b, 0x25, 0xb4, 0xaa, 0xbc, 0x62, 0xb3, 0x6b, 0x8a,
      0x05, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00};
};

TEST_CASE_METHOD(RadixFixture, "Radix has valid alphabet", "[radix]")
{
  using Catch::Matchers::Equals;

  /// @brief RFC 4648 alphabet
  const std::string base32("abcdefghijklmnopqrstuvwxyz234567");
  REQUIRE_THAT(base32, Equals(ntcp2::crypto::Base32::GetAlphabet()));

  /// @brief I2P-defined alphabet
  const std::string base64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-~");
  REQUIRE_THAT(base64, Equals(ntcp2::crypto::Base64::GetAlphabet()));
}

TEST_CASE_METHOD(RadixFixture, "Radix Base32 encodes a dest hash", "[radix]")
{
  using Catch::Matchers::Equals;

  const std::string base32(
      "s25c75a4upjbbd6gf2q34m4zwksx5zgnzjst4rzoooxxryfo4usq");

  const std::string encoded(
      ntcp2::crypto::Base32::Encode(dest_hash.data(), dest_hash.size()));

  REQUIRE_THAT(base32, Equals(encoded));

  const std::vector<std::uint8_t> decoded(
      ntcp2::crypto::Base32::Decode(encoded.c_str(), encoded.size()));

  REQUIRE_THAT(decoded, Equals(dest_hash));
}

TEST_CASE_METHOD(RadixFixture, "Radix Base64 encodes a dest hash", "[radix]")
{
  using Catch::Matchers::Equals;

  const std::string base64("lrov9Byj0hCPxi6hvjOZsqV-5M3KZT5HLnOveOCu5SU=");

  const std::string encoded(
      ntcp2::crypto::Base64::Encode(dest_hash.data(), dest_hash.size()));

  REQUIRE_THAT(base64, Equals(encoded));

  const std::vector<std::uint8_t> decoded(
      ntcp2::crypto::Base64::Decode(encoded.c_str(), encoded.size()));

  REQUIRE_THAT(decoded, Equals(dest_hash));
}

TEST_CASE_METHOD(RadixFixture, "Radix Base64 encodes full destination", "[radix]")
{
  using Catch::Matchers::Equals;

  const std::string base64("8fbeyRjtUfDUuyr9f5ZSy~cd-3GMLQxG-o7-OeBGZImvS-E2sdun1mSoLvubufNQig4J-mtC3qYzbE3NIESuuc-FrfjO69nKF0Y7QvxSJ-d8EtC59UAOix-1QSO8oP3QA914s~JImAz8MZEPbK49IWJesvB9dxsFFOA7iomZAlftw9uNE18kgBdGRw36-3tfQdVL8QBkX82hYy~rkIYATL1Zoq~15qiGUFLEG4TcEBmm1s~7qS0JJLN-UwWKT5GEBZm4EGcIcojAQ~Oz3e8KY2IV-NyD0Wa4jiuy28ULd6qqpZVAPNwf8RNeysDAeX4~CaPpZFTYKJKN~6qFpO3xLtcoJ12rMfG6uMwgO-t4QsYvmCt6MTH~kL4gz-mISv5pCGzP907Ih4DI~pkqF90rP0gh~3VIcMfAi1N8FGsZXn8CdVr5cjqIDNPmNbpxnh2iVURDE5yqbOzqTseCbk2ybzvcChTlYujMSEO1Nvxs8VX3pyrq7bcFCyW0qrxis2uKBQAEAAcAAA==");

  const std::string encoded(
      ntcp2::crypto::Base64::Encode(dest.data(), dest.size()));

  REQUIRE_THAT(base64, Equals(encoded));

  const std::vector<std::uint8_t> decoded(
      ntcp2::crypto::Base64::Decode(encoded.c_str(), encoded.size()));

  REQUIRE_THAT(decoded, Equals(dest));
}

TEST_CASE_METHOD(RadixFixture, "Radix rejects invalid encoding", "[radix]")
{
  const std::string base32("1!@#$%^&*()_-+=");
  REQUIRE_THROWS(ntcp2::crypto::Base32::Decode(base32.c_str(), base32.size()));

  const std::string base64("!@#$%^&*()_-+");
  REQUIRE_THROWS(ntcp2::crypto::Base64::Decode(base64.c_str(), base64.size()));
}

TEST_CASE_METHOD(RadixFixture, "Radix rejects null arguments", "[radix]")
{
  REQUIRE_THROWS(ntcp2::crypto::Base32::Encode(nullptr, 0));
  REQUIRE_THROWS(ntcp2::crypto::Base32::Decode(nullptr, 0));
  REQUIRE_THROWS(ntcp2::crypto::Base64::Encode(nullptr, 0));
  REQUIRE_THROWS(ntcp2::crypto::Base64::Decode(nullptr, 0));
}
