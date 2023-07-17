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
 * \brief Definition of URI class.
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Class of URIs (Uniform Resource Identifiers) to specify endpoints in
 * this library.
 */
class MSGPACK_RPC_EXPORT URI {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] schema Schema.
     * \param[in] host Host.
     * \param[in] port_number Port number.
     *
     * \note For protocols without port numbers, port_number can be omitted.
     */
    URI(std::string_view schema, std::string_view host,
        std::optional<std::uint16_t> port_number =
            std::optional<std::uint16_t>());

    /*!
     * \brief Get the schema.
     *
     * \return Schema.
     */
    [[nodiscard]] std::string_view schema() const noexcept;

    /*!
     * \brief Get the host name.
     *
     * \return Host.
     */
    [[nodiscard]] std::string_view host() const noexcept;

    /*!
     * \brief Get the port number.
     *
     * \return Port number.
     */
    [[nodiscard]] std::optional<std::uint16_t> port_number() const noexcept;

    /*!
     * \brief Parse a string to create a URI.
     *
     * \param[in] uri_string String to specify a URI.
     * \return URI.
     */
    [[nodiscard]] static URI parse(std::string_view uri_string);

private:
    //! Schema.
    std::string schema_;

    //! Host.
    std::string host_;

    //! Port.
    std::optional<std::uint16_t> port_number_;
};

}  // namespace msgpack_rpc::addresses

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::addresses::URI.
 */
template <>
class formatter<msgpack_rpc::addresses::URI> {
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
        const msgpack_rpc::addresses::URI& val, format_context& context) const;
};

}  // namespace fmt
