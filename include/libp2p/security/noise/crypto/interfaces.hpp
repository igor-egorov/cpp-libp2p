/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_CRYPTO_INTERFACES_HPP
#define LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_CRYPTO_INTERFACES_HPP

#include <tuple>

#include <boost/optional.hpp>
#include <libp2p/common/types.hpp>
#include <libp2p/crypto/common.hpp>
#include <libp2p/crypto/common_functions.hpp>
#include <libp2p/crypto/hash.hpp>
#include <libp2p/outcome/outcome.hpp>

namespace libp2p::security::noise {

  using Key32 = std::array<uint8_t, 32>;
  using libp2p::crypto::asArray;
  using libp2p::crypto::asVector;

  using crypto::common::HashType;
  using libp2p::common::ByteArray;
  struct HKDFResult {
    ByteArray one;
    ByteArray two;
    ByteArray three;
  };

  enum class HKDFError {
    ILLEGAL_OUTPUTS_NUMBER = 1,
  };

  ByteArray span2vec(gsl::span<const uint8_t> span);

  outcome::result<HKDFResult> hkdf(HashType hash_type, size_t outputs,
                                   gsl::span<const uint8_t> chaining_key,
                                   gsl::span<const uint8_t> input_key_material);

  struct DHKey {
    ByteArray priv;
    ByteArray pub;
  };

  class DiffieHellman {
   public:
    virtual ~DiffieHellman() = default;

    /// generates a key pair
    virtual outcome::result<DHKey> generate() = 0;

    /// does a Diffie-Hellman calculation between the given keys
    virtual outcome::result<ByteArray> dh(const ByteArray &private_key,
                                          const ByteArray &public_key) = 0;

    /// returns the size in bytes of the result of dh computation
    virtual int dhSize() const = 0;

    /// algorithm identifier used in Noise handshake
    virtual std::string dhName() const = 0;
  };

  class NamedHash {
   public:
    virtual ~NamedHash() = default;

    virtual std::shared_ptr<crypto::Hash> hash() = 0;

    virtual std::string hashName() const = 0;
  };

  // has to be initialized with a key
  class AEADCipher {
   public:
    virtual ~AEADCipher() = default;

    virtual outcome::result<ByteArray> encrypt(
        uint64_t nonce, gsl::span<const uint8_t> plaintext,
        gsl::span<const uint8_t> aad) = 0;

    virtual outcome::result<ByteArray> decrypt(
        uint64_t nonce, gsl::span<const uint8_t> ciphertext,
        gsl::span<const uint8_t> aad) = 0;
  };

  class NamedAEADCipher {
   public:
    virtual ~NamedAEADCipher() = default;

    virtual std::shared_ptr<AEADCipher> cipher(Key32 key) = 0;

    virtual std::string cipherName() const = 0;
  };

  class CipherSuite : public DiffieHellman,
                      public NamedHash,
                      public NamedAEADCipher {
   public:
    virtual ~CipherSuite() = default;
  };

}  // namespace libp2p::security::noise

OUTCOME_HPP_DECLARE_ERROR(libp2p::security::noise, HKDFError);

#endif  // LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_CRYPTO_INTERFACES_HPP
