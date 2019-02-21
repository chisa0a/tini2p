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

#ifndef SRC_NTCP2_DATA_PHASE_DATA_PHASE_H_
#define SRC_NTCP2_DATA_PHASE_DATA_PHASE_H_

#include <boost/any.hpp>
#include <boost/variant.hpp>

#include "src/data/blocks/date_time.h"
#include "src/data/blocks/i2np.h"
#include "src/data/blocks/options.h"
#include "src/data/blocks/padding.h"
#include "src/data/blocks/router_info.h"
#include "src/data/blocks/termination.h"

#include "src/ntcp2/data_phase/kdf.h"

namespace tini2p
{
namespace ntcp2
{
/// @struct DataPhaseMessage
struct DataPhaseMessage
{
  std::vector<std::unique_ptr<tini2p::data::Block>> blocks;
  std::vector<std::uint8_t> buffer;

  boost::endian::big_uint16_t size() const
  {
    boost::endian::big_uint16_t size = 0;
    for (const auto& block : blocks)
      size += block->size();

    return size;
  }
};

/// @class DataPhase
template <class Role_t>
class DataPhase
{
  Role_t role_;
  NoiseHandshakeState* state_;
  DataPhaseKDF kdf_;

 public:
  DataPhase(NoiseHandshakeState* state) : state_(state), kdf_(state, role_)
  {
    if (!state)
      exception::Exception{"DataPhase", __func__}
          .throw_ex<std::invalid_argument>("null handshake state.");
  }

  /// @brief Write and encrypt a message
  /// @param message DataPhase message to write
  void Write(DataPhaseMessage& message)
  {
    namespace meta = tini2p::meta::ntcp2::data_phase;
    namespace block = tini2p::meta::block;

    const exception::Exception ex{"DataPhase", __func__};

    boost::endian::big_uint16_t length = message.size();
    if (!length)
      ex.throw_ex<std::invalid_argument>("empty message.");

    length += crypto::hash::Poly1305Len;
    message.buffer.resize(meta::SizeSize + length);

    const auto dir = role_.id() == noise::InitiatorRole ? meta::BobToAlice
                                                        : meta::AliceToBob;

    tini2p::BytesWriter<decltype(message.buffer)> writer(message.buffer);

    // obfuscate message length
    kdf_.ProcessLength(length, dir);

    writer.write_bytes(length);

    bool last_block = false;
    bool term_block = false;
    for (const auto& block : message.blocks)
    {
      if (last_block)
        ex.throw_ex<std::logic_error>("padding must be the last block.");

      if (term_block && block->type() != block::PaddingID)
        ex.throw_ex<std::logic_error>(
            "termination followed by non-padding block.");

      if (block->type() == block::PaddingID)
        last_block = true;

      if (block->type() == block::TerminationID)
        term_block = true;

      block->serialize();
      writer.write_data(block->buffer());
    }

    // encrypt message in place
    noise::encrypt(
        kdf_.cipherstate(dir),
        &message.buffer[meta::SizeSize],
        message.buffer.size() - meta::SizeSize,
        ex);
  }

  /// @brief Decrypt and read a message
  /// @param message DataPhase message to read
  /// @param deobfs_len Flag to de-obfuscate the message length
  void Read(DataPhaseMessage& message, const bool deobfs_len = true)
  {
    namespace meta = tini2p::meta::ntcp2::data_phase;

    const exception::Exception ex{"DataPhase", __func__};

    auto& buf = message.buffer;
    if (buf.size() < meta::MinSize || buf.size() > meta::MaxSize)
      ex.throw_ex<std::length_error>("invalid ciphertext size.");

    boost::endian::big_uint16_t length;
    tini2p::read_bytes(buf.data(), length);

    const auto dir = role_.id() == noise::InitiatorRole ? meta::AliceToBob
                                                        : meta::BobToAlice;
    if (deobfs_len)
      kdf_.ProcessLength(length, dir);

    if (length - crypto::hash::Poly1305Len <= 0)
      return;

    if (length > meta::MaxSize - meta::SizeSize)
      ex.throw_ex<std::length_error>("invalid plaintext size.");

    noise::decrypt(kdf_.cipherstate(dir), &buf[meta::SizeSize], length, ex);

    ParseBlocks(message);
  }

  /// @brief Get a non-const reference to the KDF
  decltype(kdf_)& kdf() noexcept
  {
    return kdf_;
  }

 private:
  void ParseBlocks(DataPhaseMessage& message)
  {
    namespace block_m = tini2p::meta::block;

    using BlockPtr = std::unique_ptr<tini2p::data::Block>;

    const exception::Exception ex{"DataPhase", __func__};

    tini2p::BytesReader<decltype(message.buffer)> reader(message.buffer);
    reader.skip_bytes(block_m::SizeSize);

    bool last_block = false;

    decltype(message.blocks) blocks;

    while (reader.gcount() > crypto::hash::Poly1305Len)
      {
        std::uint8_t type;
        tini2p::read_bytes(&message.buffer[reader.count()], type);

        boost::endian::big_uint16_t size;
        tini2p::read_bytes(
            &message.buffer[reader.count() + block_m::SizeOffset], size);

        const auto b = message.buffer.begin() + reader.count();
        const auto e = b + block_m::HeaderSize + size;

        // final block(s) must be: padding or termination->padding
        //   disallows multiple padding blocks
        if (last_block && blocks.back()->type() != block_m::TerminationID)
          ex.throw_ex<std::logic_error>("invalid block ordering.");

        if (type == block_m::DateTimeID)
          blocks.emplace_back(BlockPtr(new tini2p::data::DateTimeBlock(b, e)));
        else if (type == block_m::I2NPMessageID)
          blocks.emplace_back(BlockPtr(new tini2p::data::I2NPBlock(b, e)));
        else if (type == block_m::OptionsID)
          blocks.emplace_back(BlockPtr(new tini2p::data::OptionsBlock(b, e)));
        else if (type == block_m::RouterInfoID)
          blocks.emplace_back(BlockPtr(new tini2p::data::RouterInfoBlock(b, e)));
        else if (type == block_m::PaddingID)
          {
            blocks.emplace_back(BlockPtr(new tini2p::data::PaddingBlock(b, e)));
            last_block = true;
          }
        else if (type == block_m::TerminationID)
          {
            blocks.emplace_back(BlockPtr(new tini2p::data::TerminationBlock(b, e)));
            last_block = true;
          }
        else
          ex.throw_ex<std::logic_error>("invalid block type.");

        reader.skip_bytes(e - b);
      }
    message.blocks = std::move(blocks);
  }
};
}  // namespace ntcp2
}  // namespace tini2p

#endif  // SRC_NTCP2_DATA_PHASE_DATA_PHASE_H_
