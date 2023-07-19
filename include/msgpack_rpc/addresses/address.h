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
 * \brief Definition of Address class.
 */
#pragma once

#include <utility>
#include <variant>

#include <fmt/format.h>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Class of addresses.
 */
class Address {
public:
    //! Type of variant objects of concrete addresses.
    using AddressVariant = std::variant<TCPAddress>;

    /*!
     * \brief Constructor.
     *
     * \param[in] address Address.
     */
    Address(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        TCPAddress address)
        : address_(std::move(address)) {}

    /*!
     * \brief Invoke a visitor function using the concrete address.
     *
     * \tparam Visitor Type of the visitor function.
     * \param[in] visitor Visitor function.
     * \return Return value of the visitor function.
     */
    template <typename Visitor>
    decltype(auto) visit(Visitor&& visitor) const {
        return std::visit(std::forward<Visitor>(visitor), address_);
    }

    /*!
     * \brief Convert to URI.
     *
     * \return URI.
     */
    [[nodiscard]] URI to_uri() const {
        return visit([](const auto& address) { return address.to_uri(); });
    }

    /*!
     * \brief Get as an address of TCP.
     *
     * \return Address.
     */
    [[nodiscard]] TCPAddress as_tcp() const {
        try {
            return std::get<TCPAddress>(address_);
        } catch (const std::bad_variant_access& /*exception*/) {
            throw MsgpackRPCException(
                StatusCode::PRECONDITION_NOT_MET, "Invalid type of address.");
        }
    }

private:
    //! Address.
    AddressVariant address_;
};

}  // namespace msgpack_rpc::addresses

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::addresses::Address.
 */
template <>
class formatter<msgpack_rpc::addresses::Address> {
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
        const msgpack_rpc::addresses::Address& val,
        format_context& context) const {
        return val.visit([&context](const auto& address) {
            return fmt::format_to(context.out(), "{}", address);
        });
    }
};

}  // namespace fmt
