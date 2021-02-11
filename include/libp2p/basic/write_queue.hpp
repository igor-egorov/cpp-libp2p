/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_BASIC_WRITE_QUEUE_HPP
#define LIBP2P_BASIC_WRITE_QUEUE_HPP

#include <deque>

#include <libp2p/basic/writer.hpp>

namespace libp2p::basic {

  class WriteQueue {
   public:
    using DataRef = gsl::span<const uint8_t>;

    static constexpr size_t kDefaultSizeLimit = 64 * 1024 * 1024;

    explicit WriteQueue(size_t size_limit = kDefaultSizeLimit)
        : size_limit_(size_limit) {}

    /// Enqueues data, returns false on overflow
    bool enqueue(DataRef data, bool some, basic::Writer::WriteCallbackFunc cb);

    /// Returns new window size
    size_t dequeue(size_t window_size, DataRef &out, bool &some);

    /// Calls write callback if full message was sent (or some),
    /// returns false on inconsistency
    bool ack(size_t size);

    /// Returns bool because of reentrancy and lifetime
    using BroadcastFn = std::function<bool(basic::Writer::WriteCallbackFunc)>;

    /// Helps iterate through callbacks, needed to broadcast error code
    void broadcast(const BroadcastFn &fn);

    /// Deallocates memory
    void clear();

   private:
    /// Data item w/callback
    struct Data {
      // data reference
      gsl::span<const uint8_t> data;

      // allow to send large messages partially
      size_t acknowledged;

      // was sent during write operation, not acknowledged yet
      size_t unacknowledged;

      // remaining bytes to dequeue
      size_t unsent;

      // allows to send at least 1 byte to complete operation
      bool some;

      // callback
      basic::Writer::WriteCallbackFunc cb;
    };

    size_t size_limit_;
    size_t active_index_ = 0;
    size_t total_unsent_size_ = 0;
    std::deque<Data> queue_;
  };

}  // namespace libp2p::basic

#endif  // LIBP2P_BASIC_WRITE_QUEUE_HPP
