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
 * \brief Definition of format_msgpack_object_to function.
 */
#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>  // IWYU pragma: keep
#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/util/copy_string_to_iterator.h"

namespace msgpack_rpc::util {

namespace impl {

template <typename Iterator>
Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object& object);

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, msgpack::type::nil_t /*nil*/) {
    return copy_string_to_iterator("null", iterator);
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(Iterator iterator, bool object) {
    if (object) {
        return copy_string_to_iterator("true", iterator);
    }
    return copy_string_to_iterator("false", iterator);
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator, typename T>
inline auto format_msgpack_object_impl(Iterator iterator, T object)
    -> std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
        Iterator> {
    return fmt::format_to(iterator, "{}", object);
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object_str& object) {
    *iterator = '"';
    ++iterator;
    iterator = copy_string_to_iterator(
        std::string_view(object.ptr, object.size), iterator);
    *iterator = '"';
    ++iterator;
    return iterator;
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object_bin& object) {
    iterator = copy_string_to_iterator("bin(", iterator);
    for (std::uint32_t i = 0; i < object.size; ++i) {
        const auto byte = static_cast<std::uint32_t>(
            static_cast<unsigned char>(object.ptr[i]));
        iterator = fmt::format_to(iterator, "{:02X}", byte);
    }
    *iterator = ')';
    ++iterator;
    return iterator;
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object_array& object) {
    *iterator = '[';
    ++iterator;
    for (std::uint32_t i = 0; i < object.size; ++i) {
        if (i > 0) {
            *iterator = ',';
            *iterator = ' ';
        }
        iterator = format_msgpack_object_impl(iterator, object.ptr[i]);
    }
    *iterator = ']';
    ++iterator;
    return iterator;
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object_map& object) {
    *iterator = '{';
    ++iterator;
    for (std::uint32_t i = 0; i < object.size; ++i) {
        if (i > 0) {
            *iterator = ',';
            *iterator = ' ';
        }
        iterator = format_msgpack_object_impl(iterator, object.ptr[i].key);
        *iterator = ':';
        *iterator = ' ';
        iterator = format_msgpack_object_impl(iterator, object.ptr[i].val);
    }
    *iterator = '}';
    ++iterator;
    return iterator;
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object_ext& object) {
    // NOLINTNEXTLINE(bugprone-signed-char-misuse, cert-str34-c): false positive
    const auto type_number = static_cast<int>(object.type());
    iterator = fmt::format_to(iterator, "ext({}, ", type_number);
    for (std::uint32_t i = 0; i < object.size; ++i) {
        const auto byte = static_cast<std::uint32_t>(
            static_cast<unsigned char>(object.data()[i]));
        iterator = fmt::format_to(iterator, "{:02X}", byte);
    }
    *iterator = ')';
    ++iterator;
    return iterator;
}

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_impl(
    Iterator iterator, const msgpack::object& object) {
    switch (object.type) {
    case msgpack::type::NIL:
        return format_msgpack_object_impl(iterator, msgpack::type::nil_t());
    case msgpack::type::BOOLEAN:
        return format_msgpack_object_impl(iterator, object.via.boolean);
    case msgpack::type::POSITIVE_INTEGER:
        return format_msgpack_object_impl(iterator, object.via.u64);
    case msgpack::type::NEGATIVE_INTEGER:
        return format_msgpack_object_impl(iterator, object.via.i64);
    case msgpack::type::FLOAT32:
    case msgpack::type::FLOAT64:
        return format_msgpack_object_impl(iterator, object.via.f64);
    case msgpack::type::STR:
        return format_msgpack_object_impl(iterator, object.via.str);
    case msgpack::type::BIN:
        return format_msgpack_object_impl(iterator, object.via.bin);
    case msgpack::type::ARRAY:
        return format_msgpack_object_impl(iterator, object.via.array);
    case msgpack::type::MAP:
        return format_msgpack_object_impl(iterator, object.via.map);
    case msgpack::type::EXT:
        return format_msgpack_object_impl(iterator, object.via.ext);
    default:
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Invalid message type.");
    }
}

}  // namespace impl

/*!
 * \brief Format an object in msgpack library.
 *
 * \tparam Iterator Type of the output iterator of the formatted string.
 * \param[out] iterator Output iterator of the formatted string.
 * \param[in] object Object in msgpack library.
 * \return Output iterator after formatting the object.
 */
template <typename Iterator>
inline Iterator format_msgpack_object_to(
    Iterator iterator, const msgpack::object& object) {
    return impl::format_msgpack_object_impl(iterator, object);
}

}  // namespace msgpack_rpc::util
