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
 * \brief Definition of TCPAddress.
 */
#pragma once

#include <cstdint>
#include <string_view>

#include <asio/ip/tcp.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Type of addresses of TCP in asio library.
 */
using AsioTCPAddress = asio::ip::tcp::endpoint;

/*!
 * \brief Class of addresses of TCP.
 */
class MSGPACK_RPC_EXPORT TCPAddress {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] ip_address IP address.
     * \param[in] port_number Port number.
     */
    TCPAddress(std::string_view ip_address, std::uint16_t port_number);

    /*!
     * \brief Constructor.
     *
     * \param[in] address Address in asio library.
     */
    explicit TCPAddress(AsioTCPAddress address);

    /*!
     * \brief Get the IP address.
     *
     * \return IP address.
     */
    [[nodiscard]] std::string ip_address() const;

    /*!
     * \brief Get the port number.
     *
     * \return Port number.
     */
    [[nodiscard]] std::uint16_t port_number() const;

    /*!
     * \brief Get the address in asio library.
     *
     * \return Address in asio library.
     */
    [[nodiscard]] const AsioTCPAddress& asio_address() const;

private:
    //! Address in asio library.
    AsioTCPAddress address_;
};

}  // namespace msgpack_rpc::addresses

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::addresses::TCPAddress.
 */
template <>
class formatter<msgpack_rpc::addresses::TCPAddress> {
public:
    /*!
     * \brief Parse format.
     *
     * \param[in] context Context.
     * \return Iterator of the format.
     */
    constexpr format_parse_context::iterator parse(  // NOLINT
        format_parse_context& context) {
        return context.end();
    }

    /*!
     * \brief Format a value.
     *
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Iterator of the buffer.
     */
    format_context::iterator format(  // NOLINT
        const msgpack_rpc::addresses::TCPAddress& val,
        format_context& context) const {
        return fmt::format_to(
            context.out(), "tcp://{}", fmt::streamed(val.asio_address()));
    }
};

}  // namespace fmt
