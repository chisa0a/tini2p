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

#include "tests/unit_tests/mock/handshake.h"

struct DataPhaseFixture : public MockHandshake
{
  using Info = tini2p::data::Info;

  using BlockPtr = tini2p::data::Block::pointer;
  using DateTimeBlock = tini2p::data::DateTimeBlock;
  using PaddingBlock = tini2p::data::PaddingBlock;
  using RouterInfoBlock = tini2p::data::RouterInfoBlock;
  using TerminationBlock = tini2p::data::TerminationBlock;

  DataPhaseFixture()
  {
    ValidSessionRequest();
    ValidSessionCreated();
    ValidSessionConfirmed();
    InitializeDataPhase();
  }

  std::unique_ptr<Info> ri;
};

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase initiator and responder encrypt and decrypt a message",
    "[dp]")
{
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));

  REQUIRE_NOTHROW(dp_initiator->Write(dp_message));
  REQUIRE_NOTHROW(dp_responder->Read(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase responder encrypts and decrypts a message with blocks",
    "[dp]")
{
  ri = std::make_unique<Info>();
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));
  dp_message.blocks.emplace_back(BlockPtr(new RouterInfoBlock(ri.get())));
  dp_message.blocks.emplace_back(BlockPtr(new PaddingBlock(17)));

  REQUIRE_NOTHROW(dp_initiator->Write(dp_message));
  REQUIRE_NOTHROW(dp_responder->Read(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase initiator and responder reject writing empty messages",
    "[dp]")
{
  REQUIRE_THROWS(dp_initiator->Write(dp_message));
  REQUIRE_THROWS(dp_responder->Write(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase initiator and responder reject invalid MAC",
    "[dp]")
{
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));

  REQUIRE_NOTHROW(dp_initiator->Write(dp_message));

  // invalidate ciphertext
  crypto::RandBytes(dp_message.buffer);
  REQUIRE_THROWS(dp_responder->Read(dp_message));

  REQUIRE_NOTHROW(dp_responder->Write(dp_message));

  // invalidate ciphertext
  crypto::RandBytes(dp_message.buffer);
  REQUIRE_THROWS(dp_initiator->Read(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase initiator and responder reject invalid block order",
    "[dp]")
{
  // invalid order, padding must be last block
  dp_message.blocks.emplace_back(BlockPtr(new PaddingBlock(3)));
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));

  REQUIRE_THROWS(dp_initiator->Write(dp_message));
  REQUIRE_THROWS(dp_responder->Write(dp_message));

  dp_message.blocks.clear();

  // invalid order, termination must only be followed by padding block
  dp_message.blocks.emplace_back(BlockPtr(new TerminationBlock()));
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));

  REQUIRE_THROWS(dp_initiator->Write(dp_message));
  REQUIRE_THROWS(dp_responder->Write(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase initiator and responder reject invalid size",
    "[dp]")
{
  namespace block_m = tini2p::meta::block;

  // add blocks to make message oversized
  dp_message.blocks.emplace_back(BlockPtr(new TerminationBlock()));

  reinterpret_cast<TerminationBlock*>(dp_message.blocks.back().get())
      ->add_data()
      .resize(block_m::MaxTermAddDataSize);

  dp_message.blocks.emplace_back(
      BlockPtr(new PaddingBlock(block_m::MaxPaddingSize)));

  REQUIRE_THROWS(dp_initiator->Write(dp_message));
  REQUIRE_THROWS(dp_responder->Write(dp_message));

  dp_message.blocks.clear();
  dp_message.blocks.emplace_back(BlockPtr(new DateTimeBlock()));
  REQUIRE_NOTHROW(dp_initiator->Write(dp_message));

  // invalidate the size in the raw message buffer
  crypto::RandBytes(dp_message.buffer);
  REQUIRE_THROWS(dp_responder->Read(dp_message));
}

TEST_CASE_METHOD(
    DataPhaseFixture,
    "DataPhase rejects null handshake state",
    "[dp]")
{
  REQUIRE_THROWS(sess_init_t::data_impl_t(nullptr));
  REQUIRE_THROWS(sess_resp_t::data_impl_t(nullptr));
}
