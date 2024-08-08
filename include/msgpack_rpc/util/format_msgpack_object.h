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
 * \brief Definition of format_msgpack_object function.
 */
#pragma once

#include <fmt/base.h>
#include <msgpack.hpp>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::util::impl {

/*!
 * \brief Class to format objects in msgpack library using fmt library.
 */
class MSGPACK_RPC_EXPORT FmtMsgpackObjectProxy {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] object Object in msgpack library.
     */
    explicit FmtMsgpackObjectProxy(const msgpack::object& object);

    /*!
     * \brief Get the object in msgpack library.
     *
     * \return Object in msgpack library.
     */
    [[nodiscard]] const msgpack::object& object() const noexcept;

private:
    //! Object in msgpack library.
    const msgpack::object&
        object_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
                  // This class is used only as temporary objects.
};

}  // namespace msgpack_rpc::util::impl

namespace fmt {

/*!
 * \brief Specialization of fmt::format for
 * msgpack_rpc::util::impl::FmtMsgpackObjectProxy.
 */
template <>
struct formatter<msgpack_rpc::util::impl::FmtMsgpackObjectProxy> {
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
     * \param[in] value Value.
     * \param[in] context Context.
     * \return Iterator of the buffer.
     */
    [[nodiscard]] MSGPACK_RPC_EXPORT format_context::iterator format(
        const msgpack_rpc::util::impl::FmtMsgpackObjectProxy& value,
        format_context& context) const;
};

}  // namespace fmt

namespace msgpack_rpc::util {

/*!
 * \brief Format an object in msgpack library using fmt library.
 *
 * \param[in] object Object in msgpack library.
 * \return Proxy object to use in fmt library.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT impl::FmtMsgpackObjectProxy
format_msgpack_object(const msgpack::object& object);

}  // namespace msgpack_rpc::util
