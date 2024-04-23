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
 * \brief Definition of MethodException class.
 */
#pragma once

#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

#include <msgpack.hpp>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Class of exceptions in methods.
 *
 * \note Methods can throw this exception to notify errors using any
 * serializable objects.
 */
class MSGPACK_RPC_EXPORT MethodException : public std::exception {
public:
    // Exceptions must be exported in libraries. If not exported, exceptions
    // cannot be caught correctly.

    /*!
     * \brief Constructor.
     *
     * \tparam T Object specifying the error.
     * \param object Object specifying the error.
     *
     * \note The second argument is for SFINAE, so the second argument should be
     * left unspecified.
     */
    template <typename T>
    explicit MethodException(T&& object,
        std::enable_if_t<!std::is_same_v<std::decay_t<T>, MethodException>,
            void*> /*for SFINAE*/
        = nullptr)
        : zone_(std::make_shared<msgpack::zone>()),
          object_(std::forward<T>(object), *zone_) {}

    /*!
     * \brief Get the object in msgpack library.
     *
     * \return Object in msgpack library.
     */
    [[nodiscard]] const msgpack::object& object() const noexcept;

    /*!
     * \brief Get the message of this exception.
     *
     * \return Message.
     */
    [[nodiscard]] const char* what() const noexcept override;

    /*!
     * \brief Copy constructor.
     */
    MethodException(const MethodException&) noexcept;

    /*!
     * \brief Move constructor.
     */
    MethodException(MethodException&&) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    MethodException& operator=(const MethodException&) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    MethodException& operator=(MethodException&&) noexcept;

    //! Destructor.
    ~MethodException() noexcept override;

private:
    //! Zone in msgpack library.
    std::shared_ptr<msgpack::zone> zone_;

    //! Object in msgpack library.
    msgpack::object object_;
};

}  // namespace msgpack_rpc::methods
