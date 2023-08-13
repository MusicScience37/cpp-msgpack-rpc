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
 * \brief Definition of MethodName class.
 */
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/core.h>

namespace msgpack_rpc::messages {

/*!
 * \brief Class of method names.
 */
class MethodName {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     *
     * \note This class saves pointer to the give buffer, and doesn't manage
     * memory.
     */
    MethodName(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        std::string name)
        : name_(std::move(name)) {
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
    MethodName(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
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
    MethodName(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const char* name, std::size_t size)
        : name_(name, size) {
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
    MethodName(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const char* name)
        : name_(name) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] std::string_view name() const noexcept { return name_; }

private:
    //! Method name.
    std::string name_;
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
    const MethodName& left, const MethodName& right) noexcept {
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
    const MethodName& left, const MethodName& right) noexcept {
    return left.name() != right.name();
}

}  // namespace msgpack_rpc::messages

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for
 * msgpack_rpc::messages::MethodName.
 */
template <>
class formatter<msgpack_rpc::messages::MethodName>
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
        const msgpack_rpc::messages::MethodName& val,
        format_context& context) const {
        return formatter<std::string_view>::format(val.name(), context);
    }
};

}  // namespace fmt
