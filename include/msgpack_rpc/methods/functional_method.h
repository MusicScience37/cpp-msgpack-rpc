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
 * \brief Definition of FunctionalMethod class.
 */
#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/method_exception.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Class of methods implemented by function objects.
 *
 * \tparam Signature Signature of the method.
 * \tparam Function Type of the function implementing the method.
 */
template <typename Signature, typename Function, typename /*for SFINAE*/ = void>
class FunctionalMethod;

/*!
 * \brief Class of methods implemented by function objects.
 *
 * \tparam Function Type of the function implementing the method.
 * \tparam Return Type of return values of the method.
 * \tparam Parameters Types of parameters of the method.
 */
template <typename Function, typename Return, typename... Parameters>
class FunctionalMethod<Return(Parameters...), Function,
    std::enable_if_t<!std::is_same_v<Return, void>>> {
public:
    /*!
     * \brief Constructor.
     *
     * \tparam InputFunction Type of the function in the argument.
     * \param[in] name Method name.
     * \param[in] function Function implementing the method.
     * \param[in] logger Logger.
     */
    template <typename InputFunction>
    FunctionalMethod(messages::MethodNameView name, InputFunction&& function,
        std::shared_ptr<logging::Logger> logger)
        : name_(name),
          function_(std::forward<InputFunction>(function)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] messages::MethodNameView name() const noexcept {
        return name_;
    }

    /*!
     * \brief Call this method.
     *
     * \param[in] request Request.
     * \return Serialized response.
     */
    [[nodiscard]] messages::SerializedMessage call(
        const messages::ParsedRequest& request) {
        try {
            return messages::MessageSerializer::serialize_successful_response(
                request.id(),
                invoke_with_tuple(
                    request.parameters().as<std::decay_t<Parameters>...>()));
        } catch (const MethodException& exception) {
            return messages::MessageSerializer::serialize_error_response(
                request.id(), exception.object());
        }
    }

    /*!
     * \brief Notify this method.
     *
     * \param[in] request Request.
     */
    void notify(const messages::ParsedRequest& request) {
        try {
            std::apply(function_,
                request.parameters().as<std::decay_t<Parameters>...>());
        } catch (const MethodException& /*exception*/) {
            MSGPACK_RPC_DEBUG(logger_,
                "Method {} threw an exception with a custom object.", name_);
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(
                logger_, "Method {} threw an exception: {}", name_, e.what());
        }
    }

private:
    /*!
     * \brief Invoke the function with a tuple of parameters.
     *
     * \tparam ParameterTuple Type of the tuple of parameters.
     * \param[in] parameter_tuple Tuple of parameters.
     * \return Return value.
     * \throws MethodException Thrown when an exception is thrown by the
     * function.
     */
    template <typename ParameterTuple>
    decltype(auto) invoke_with_tuple(ParameterTuple&& parameter_tuple) {
        try {
            return std::apply(
                function_, std::forward<ParameterTuple>(parameter_tuple));
        } catch (const MethodException& /*exception*/) {
            MSGPACK_RPC_DEBUG(logger_,
                "Method {} threw an exception with a custom object.", name_);
            throw;
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(
                logger_, "Method {} threw an exception: {}", name_, e.what());
            throw MethodException(e.what());
        }
    }

    //! Method name.
    messages::MethodName name_;

    //! Function.
    std::decay_t<Function> function_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

/*!
 * \brief Class of methods implemented by function objects.
 *
 * \tparam Function Type of the function implementing the method.
 * \tparam Parameters Types of parameters of the method.
 */
template <typename Function, typename... Parameters>
class FunctionalMethod<void(Parameters...), Function> {
public:
    /*!
     * \brief Constructor.
     *
     * \tparam InputFunction Type of the function in the argument.
     * \param[in] name Method name.
     * \param[in] function Function implementing the method.
     * \param[in] logger Logger.
     */
    template <typename InputFunction>
    FunctionalMethod(messages::MethodNameView name, InputFunction&& function,
        std::shared_ptr<logging::Logger> logger)
        : name_(name),
          function_(std::forward<InputFunction>(function)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] messages::MethodNameView name() const noexcept {
        return name_;
    }

    /*!
     * \brief Call this method.
     *
     * \param[in] request Request.
     * \return Serialized response.
     */
    [[nodiscard]] messages::SerializedMessage call(
        const messages::ParsedRequest& request) {
        try {
            std::apply(function_,
                request.parameters().as<std::decay_t<Parameters>...>());
        } catch (const MethodException& e) {
            MSGPACK_RPC_DEBUG(logger_,
                "Method {} threw an exception with a custom object.", name_);
            return messages::MessageSerializer::serialize_error_response(
                request.id(), e.object());
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(
                logger_, "Method {} threw an exception: {}", name_, e.what());
            return messages::MessageSerializer::serialize_error_response(
                request.id(), e.what());
        }
        return messages::MessageSerializer::serialize_successful_response(
            request.id(), msgpack::type::nil_t());
    }

    /*!
     * \brief Notify this method.
     *
     * \param[in] request Request.
     */
    void notify(const messages::ParsedRequest& request) {
        try {
            std::apply(function_,
                request.parameters().as<std::decay_t<Parameters>...>());
        } catch (const MethodException& /*exception*/) {
            MSGPACK_RPC_DEBUG(logger_,
                "Method {} threw an exception with a custom object.", name_);
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(
                logger_, "Method {} threw an exception: {}", name_, e.what());
        }
    }

private:
    //! Method name.
    messages::MethodName name_;

    //! Function.
    std::decay_t<Function> function_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

/*!
 * \brief Create a method implemented by a function object.
 *
 * \tparam Signature Signature of the method.
 * \tparam Function Type of the function implementing the method.
 * \param[in] name Name of the method.
 * \param[in] function Function implementing the method.
 * \param[in] logger Logger.
 * \return Method.
 */
template <typename Signature, typename Function>
[[nodiscard]] inline std::unique_ptr<FunctionalMethod<Signature, Function>>
create_functional_method(messages::MethodNameView name, Function&& function,
    std::shared_ptr<logging::Logger> logger) {
    return std::make_unique<
        FunctionalMethod<Signature, std::decay_t<Function>>>(
        name, std::forward<Function>(function), std::move(logger));
}

}  // namespace msgpack_rpc::methods
