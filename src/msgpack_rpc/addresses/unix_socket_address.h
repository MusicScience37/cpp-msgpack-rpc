/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of UnixSocketAddress class.
 */
#pragma once

#include "msgpack_rpc/impl/config.h"

#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS

#include <asio/local/stream_protocol.hpp>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Type of addresses of Unix sockets in asio library.
 */
using AsioUnixSocketAddress = asio::local::stream_protocol::endpoint;

/*!
 * \brief Class of address of Unix sockets.
 */
class MSGPACK_RPC_EXPORT UnixSocketAddress final : public IAddress {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] address Address in asio library.
     *
     * \note The argument can be `std::string` or `const char*`, because
     * AsioUnixSocketAddress can implicitly constructed from them.
     */
    explicit UnixSocketAddress(AsioUnixSocketAddress address);

    /*!
     * \brief Get the filepath.
     *
     * \return Filepath.
     */
    [[nodiscard]] std::string filepath() const;

    //! \copydoc msgpack_rpc::addresses::IAddress::to_uri
    [[nodiscard]] URI to_uri() const override;

    //! \copydoc msgpack_rpc::addresses::IAddress::to_string
    [[nodiscard]] std::string to_string() const override;

    /*!
     * \brief Get the address in asio library.
     *
     * \return Address in asio library.
     */
    [[nodiscard]] const AsioUnixSocketAddress& asio_address() const;

    /*!
     * \brief Compare with an address.
     *
     * \param[in] right Right-hand-side address.
     * \retval true Two addresses are same.
     * \retval false Two addresses are different.
     */
    [[nodiscard]] bool operator==(const UnixSocketAddress& right) const;

    /*!
     * \brief Compare with an address.
     *
     * \param[in] right Right-hand-side address.
     * \retval true Two addresses are different.
     * \retval false Two addresses are same.
     */
    [[nodiscard]] bool operator!=(const UnixSocketAddress& right) const;

private:
    //! Address in asio library.
    AsioUnixSocketAddress address_;
};

}  // namespace msgpack_rpc::addresses

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::addresses::UnixSocketAddress.
 */
template <>
class formatter<msgpack_rpc::addresses::UnixSocketAddress> {
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
    MSGPACK_RPC_EXPORT format_context::iterator format(  // NOLINT
        const msgpack_rpc::addresses::UnixSocketAddress& val,
        format_context& context) const;
};

}  // namespace fmt

/*!
 * \brief Format an address.
 *
 * \param[in] stream Stream.
 * \param[in] address Address.
 * \return Stream after formatting.
 */
MSGPACK_RPC_EXPORT std::ostream& operator<<(std::ostream& stream,
    const msgpack_rpc::addresses::UnixSocketAddress& address);

#endif
