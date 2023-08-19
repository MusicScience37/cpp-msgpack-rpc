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
 * \brief Definition of MethodNameView class.
 */
#pragma once

// IWYU pragma: no_include <typeindex>

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>

#include <fmt/core.h>
// This header is required for formatter<std::string_view>
#include <fmt/format.h>  // IWYU pragma: keep

#include "msgpack_rpc/messages/method_name.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of method names.
 */
class MethodNameView {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodNameView(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const MethodName& name) noexcept
        : name_(name.name()) {
        // Validation is done in MethodName.
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodNameView(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const std::string& name)
        : name_(name) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodNameView(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        std::string_view name)
        : name_(name) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     * \param[in] size Number of bytes in the name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodNameView(const char* name, std::size_t size) : name_(name, size) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodNameView(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const char* name)
        : name_(name) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Convert to MethodName.
     *
     * \return MethodName object.
     */
    explicit operator MethodName() const { return MethodName(name_); }

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] std::string_view name() const noexcept { return name_; }

private:
    //! Method name.
    std::string_view name_;
};

/*!
 * \brief Compare two method names.
 *
 * \param[in] left Left-hand-side object.
 * \param[in] right Right-hand-side object.
 * \retval true Two names are equal.
 * \retval false Two names are different.
 */
[[nodiscard]] inline bool operator==(
    MethodNameView left, MethodNameView right) noexcept {
    return left.name() == right.name();
}

/*!
 * \brief Compare two method names.
 *
 * \param[in] left Left-hand-side object.
 * \param[in] right Right-hand-side object.
 * \retval true Two names are different.
 * \retval false Two names are equal.
 */
[[nodiscard]] inline bool operator!=(
    MethodNameView left, MethodNameView right) noexcept {
    return left.name() != right.name();
}

}  // namespace msgpack_rpc::messages

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::messages::MethodNameView.
 */
template <>
class formatter<msgpack_rpc::messages::MethodNameView>
    : public formatter<std::string_view> {
public:
    /*!
     * \brief Format a value.
     *
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Iterator of the buffer.
     */
    format_context::iterator format(
        const msgpack_rpc::messages::MethodNameView& val,
        format_context& context) const {
        return formatter<std::string_view>::format(val.name(), context);
    }
};

}  // namespace fmt

namespace std {

/*!
 * \brief Specialization of std::hash for msgpack_rpc::messages::MethodNameView.
 */
template <>
struct hash<msgpack_rpc::messages::MethodNameView> {
public:
    /*!
     * \brief Calculate the hash number of a value.
     *
     * \param[in] value Value.
     * \return Hash number.
     */
    std::size_t operator()(
        const msgpack_rpc::messages::MethodNameView& value) const {
        return string_view_hash_(value.name());
    }

private:
    //! Hash of string_view.
    std::hash<std::string_view> string_view_hash_{};
};

}  // namespace std
