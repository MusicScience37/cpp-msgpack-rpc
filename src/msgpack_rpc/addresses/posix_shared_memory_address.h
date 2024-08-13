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
 * \brief Definition of PosixSharedMemoryAddress class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_SHM

#include <ostream>
#include <string>

#include <fmt/base.h>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Class of addresses of POSIX shared memory.
 */
class MSGPACK_RPC_EXPORT PosixSharedMemoryAddress final : public IAddress {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file_name File name.
     */
    explicit PosixSharedMemoryAddress(std::string file_name);

    /*!
     * \brief Get the file name.
     *
     * \return File name.
     */
    [[nodiscard]] const std::string& file_name() const noexcept;

    //! \copydoc msgpack_rpc::addresses::IAddress::to_uri
    [[nodiscard]] URI to_uri() const override;

    //! \copydoc msgpack_rpc::addresses::IAddress::to_string
    [[nodiscard]] std::string to_string() const override;

    /*!
     * \brief Compare with an address.
     *
     * \param[in] right Right-hand-side address.
     * \retval true Two addresses are same.
     * \retval false Two addresses are different.
     */
    [[nodiscard]] bool operator==(const PosixSharedMemoryAddress& right) const;

    /*!
     * \brief Compare with an address.
     *
     * \param[in] right Right-hand-side address.
     * \retval true Two addresses are different.
     * \retval false Two addresses are same.
     */
    [[nodiscard]] bool operator!=(const PosixSharedMemoryAddress& right) const;

private:
    //! File name.
    std::string file_name_;
};

/*!
 * \brief Format an address.
 *
 * \param[in] stream Stream.
 * \param[in] address Address.
 * \return Stream after formatting.
 */
MSGPACK_RPC_EXPORT std::ostream& operator<<(std::ostream& stream,
    const msgpack_rpc::addresses::PosixSharedMemoryAddress& address);

}  // namespace msgpack_rpc::addresses

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::addresses::PosixSharedMemoryAddress.
 */
template <>
class formatter<msgpack_rpc::addresses::PosixSharedMemoryAddress> {
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
        const msgpack_rpc::addresses::PosixSharedMemoryAddress& val,
        format_context& context) const;
};

}  // namespace fmt

#endif
