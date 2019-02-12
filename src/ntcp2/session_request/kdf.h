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

#ifndef SRC_NTCP2_SESSION_REQUEST_KDF_H_
#define SRC_NTCP2_SESSION_REQUEST_KDF_H_

#include <noise/protocol/handshakestate.h>

#include "src/exception/exception.h"

#include "src/crypto/meta.h"

#include "src/ntcp2/noise.h"
#include "src/ntcp2/meta.h"
#include "src/ntcp2/role.h"

namespace ntcp2
{
class SessionRequestKDF
{
  NoiseHandshakeState* state_;

 public:
  SessionRequestKDF(NoiseHandshakeState* state) : state_(state) {}

  /// @brief Set the responder's remote public key
  /// @param key Remote public key
  /// @detail Validates the public key internally
  void set_remote_key(const ntcp2::crypto::pk::X25519& key)
  {
    noise::set_remote_public_key(state_, key, {"SessionRequestKDF", __func__});
  }

  /// @brief Get the local static public key
  /// @param key Key to store the local static public key
  void get_local_public_key(ntcp2::crypto::pk::X25519& key) const
  {
    noise::get_local_public_key(state_, key, {"SessionRequestKDF", __func__});
  }

  /// @brief Set the local static keys
  /// @param keys Keypair to set
  void set_local_keys(const crypto::x25519::Keypair& keys)
  {
    noise::set_local_keypair(state_, keys, {"SessionRequestKDF", __func__});
  }

  /// @brief Generate local static keys
  void generate_keys()
  {
    noise::generate_keypair(state_, {"SessionRequestKDF", __func__});
  }

  /// @brief Convenience function to derive keys for this session request
  /// @param key Remote static key used for Diffie-Hellman
  void derive_keys(const ntcp2::crypto::pk::X25519& key)
  {
    set_remote_key(key);
    derive_keys();
  }

  /// @brief Performs final steps in key derivation
  /// @detail On success, handshake state is ready to write the first message
  void derive_keys()
  {
    if (const int err = noise_handshakestate_start(state_))
      exception::Exception{"SessionRequestKDF", __func__}
          .throw_ex<std::runtime_error>(
              "unable to derive session request keys", err);
  }
};
}  // namespace ntcp2

#endif  // SRC_NTCP2_SESSION_REQUEST_KDF_H_
