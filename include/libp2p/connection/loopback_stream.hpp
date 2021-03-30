/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_CONNECTION_LOOPBACKSTREAM
#define LIBP2P_CONNECTION_LOOPBACKSTREAM

#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <libp2p/connection/stream.hpp>

#include <libp2p/log/logger.hpp>
#include <libp2p/outcome/outcome.hpp>
#include <libp2p/peer/peer_info.hpp>

namespace libp2p::connection {

  class LoopbackStream : public libp2p::connection::Stream,
                         public std::enable_shared_from_this<LoopbackStream> {
   public:
    enum class Error {
      INVALID_ARGUMENT = 1,
      IS_CLOSED_FOR_READS,
      IS_CLOSED_FOR_WRITES,
      IS_RESET,
      INTERNAL_ERROR
    };

    explicit LoopbackStream(libp2p::peer::PeerInfo own_peer_info);

    bool isClosedForRead() const override;
    bool isClosedForWrite() const override;
    bool isClosed() const override;

    void close(VoidResultHandlerFunc cb) override;
    void reset() override;

    void adjustWindowSize(uint32_t new_size, VoidResultHandlerFunc cb) override;

    outcome::result<bool> isInitiator() const override;

    outcome::result<libp2p::peer::PeerId> remotePeerId() const override;

    outcome::result<libp2p::multi::Multiaddress> localMultiaddr()
        const override;

    outcome::result<libp2p::multi::Multiaddress> remoteMultiaddr()
        const override;

   protected:
    void read(gsl::span<uint8_t> out, size_t bytes,
              ReadCallbackFunc cb) override;

    void readSome(gsl::span<uint8_t> out, size_t bytes,
                  ReadCallbackFunc cb) override;

    void write(gsl::span<const uint8_t> in, size_t bytes,
               WriteCallbackFunc cb) override;

    void writeSome(gsl::span<const uint8_t> in, size_t bytes,
                   WriteCallbackFunc cb) override;

   private:
    void read(gsl::span<uint8_t> out, size_t bytes, ReadCallbackFunc cb,
              bool some);

    libp2p::peer::PeerInfo own_peer_info_;

    log::Logger log_ = log::createLogger("LoopbackStream", "network");

    /// data, received for this stream, comes here
    boost::asio::streambuf buffer_;

    /// when a new data arrives, this function is to be called
    std::function<void(outcome::result<size_t>)> data_notifyee_;
    bool data_notified_ = false;

    /// is the stream opened for reads?
    bool is_readable_ = true;

    /// is the stream opened for writes?
    bool is_writable_ = true;

    /// was the stream reset?
    bool is_reset_ = false;
  };

}  // namespace libp2p::connection

OUTCOME_HPP_DECLARE_ERROR(libp2p::connection, LoopbackStream::Error);

#endif  // LIBP2P_CONNECTION_LOOPBACKSTREAM
