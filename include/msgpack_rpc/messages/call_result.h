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
 * \brief Definition of CallResult class.
 */
#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of results of methods.
 */
class CallResult {
public:
    /*!
     * \brief Create an successful result.
     *
     * \param[in] object Object in msgpack library.
     * \param[in] zone Zone in msgpack library.
     * \return Result.
     */
    [[nodiscard]] static CallResult create_result(
        msgpack::object object, std::shared_ptr<msgpack::zone> zone) {
        constexpr bool is_error = false;
        return CallResult(is_error, object, std::move(zone));
    }

    /*!
     * \brief Create an error result.
     *
     * \param[in] object Object in msgpack library.
     * \param[in] zone Zone in msgpack library.
     * \return Result.
     */
    [[nodiscard]] static CallResult create_error(
        msgpack::object object, std::shared_ptr<msgpack::zone> zone) {
        constexpr bool is_error = true;
        return CallResult(is_error, object, std::move(zone));
    }

    /*!
     * \brief Check whether this object has an error.
     *
     * \return Whether this object has an error.
     */
    [[nodiscard]] bool is_error() const noexcept { return is_error_; }

    /*!
     * \brief Check whether this object has a successful result.
     *
     * \return Whether this object has an successful result.
     */
    [[nodiscard]] bool is_success() const noexcept { return !is_error_; }

    /*!
     * \brief Get the error.
     *
     * \tparam T Type.
     * \return Error.
     */
    template <typename T>
    [[nodiscard]] T error_as() const {
        if (!is_error_) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This result is not an error.");
        }
        try {
            return object_.as<T>();
        } catch (const msgpack::type_error&) {
            throw MsgpackRPCException(
                StatusCode::TYPE_ERROR, "Invalid type of the error.");
        }
    }

    /*!
     * \brief Get the result.
     *
     * \tparam T Type.
     * \return Result.
     */
    template <typename T>
    [[nodiscard]] T result_as() const {
        if (is_error_) {
            throw MsgpackRPCException(
                StatusCode::PRECONDITION_NOT_MET, "This result is an error.");
        }
        try {
            return object_.as<T>();
        } catch (const msgpack::type_error&) {
            throw MsgpackRPCException(
                StatusCode::TYPE_ERROR, "Invalid type of the result.");
        }
    }

    /*!
     * \brief Get the internal object in msgpack library.
     *
     * \return Object.
     */
    [[nodiscard]] msgpack::object object() const noexcept { return object_; }

    /*!
     * \brief Get the internal zone in msgpack library.
     *
     * \return Zone.
     */
    [[nodiscard]] std::shared_ptr<msgpack::zone> zone() const noexcept {
        return zone_;
    }

    /*!
     * \brief Copy constructor.
     */
    CallResult(const CallResult&) = default;

    /*!
     * \brief Move constructor.
     */
    CallResult(CallResult&&) noexcept = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    CallResult& operator=(const CallResult&) = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    CallResult& operator=(CallResult&&) noexcept = default;

    /*!
     * \brief Destructor.
     */
    ~CallResult() = default;

private:
    /*!
     * \brief Constructor.
     *
     * \param[in] is_error Whether this is an error.
     * \param[in] object Object in msgpack library.
     * \param[in] zone Zone in msgpack library.
     */
    CallResult(bool is_error, msgpack::object object,
        std::shared_ptr<msgpack::zone> zone)
        : is_error_(is_error), object_(object), zone_(std::move(zone)) {}

    //! Whether this is an error.
    bool is_error_;

    //! Object in msgpack library.
    msgpack::object object_;

    //! Zone in msgpack library.
    std::shared_ptr<msgpack::zone> zone_;
};

}  // namespace msgpack_rpc::messages
