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
 * \brief Definition of MessageParser class.
 */
#pragma once

#include <cstddef>
#include <optional>

#include <msgpack.hpp>
#include <msgpack/v1/unpack_decl.hpp>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/messages/buffer_view.h"
#include "msgpack_rpc/messages/parsed_message.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class to parse messages.
 */
class MSGPACK_RPC_EXPORT MessageParser {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] config Configuration.
     */
    explicit MessageParser(const config::MessageParserConfig& config);

    MessageParser(const MessageParser&) = delete;
    MessageParser(MessageParser&&) = delete;
    MessageParser& operator=(const MessageParser&) = delete;
    MessageParser& operator=(MessageParser&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~MessageParser();

    /*!
     * \brief Prepare a buffer.
     *
     * \return Buffer.
     */
    BufferView prepare_buffer();

    /*!
     * \brief Set some bytes to be consumed.
     *
     * \param[in] num_bytes Number of consumed bytes.
     */
    void consumed(std::size_t num_bytes);

    /*!
     * \brief Try to parse a message and return it if parsed, throw an exception
     * if the message data is invalid.
     *
     * \return Message if parsed. Null if more data is required.
     */
    [[nodiscard]] std::optional<ParsedMessage> try_parse();

private:
    //! Parser.
    msgpack::unpacker parser_;

    //! Buffer size to read at once.
    std::size_t read_buffer_size_;
};

}  // namespace msgpack_rpc::messages
