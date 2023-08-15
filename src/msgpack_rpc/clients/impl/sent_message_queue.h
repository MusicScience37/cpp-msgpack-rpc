/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of SentMessageQueue class.
 */
#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <tuple>
#include <utility>

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of queues of messages to be sent.
 */
class SentMessageQueue {
public:
    //! Constructor.
    SentMessageQueue() = default;

    /*!
     * \brief Get the next message.
     *
     * \return Next message and its message ID if exists.
     */
    [[nodiscard]] std::tuple<std::shared_ptr<messages::SerializedMessage>,
        std::optional<messages::MessageID>>
    next() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return {nullptr, std::nullopt};
        }
        return queue_.front();
    }

    /*!
     * \brief Pop a message.
     */
    void pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.pop();
    }

    /*!
     * \brief Push a message.
     *
     * \param[in] message Message.
     * \param[in] id Message ID (for requests).
     */
    void push(std::shared_ptr<messages::SerializedMessage> message,
        std::optional<messages::MessageID> id = std::nullopt) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(std::move(message), id);
    }

private:
    //! Queue.
    std::queue<std::tuple<std::shared_ptr<messages::SerializedMessage>,
        std::optional<messages::MessageID>>>
        queue_{};

    //! Mutex.
    std::mutex mutex_{};
};

}  // namespace msgpack_rpc::clients::impl
