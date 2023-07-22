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

#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::methods {

template <typename Signature, typename Function>
class FunctionalMethod;

/*!
 * \brief Class of methods implemented by function objects.
 *
 * \tparam Function Type of the function implementing the method.
 * \tparam Return Type of return values of the method.
 * \tparam Parameters Types of parameters of the method.
 */
template <typename Function, typename Return, typename... Parameters>
class FunctionalMethod<Return(Parameters...), Function> {
public:
    /*!
     * \brief Constructor.
     *
     * \tparam InputFunction Type of the function in the argument.
     * \param[in] name Method name.
     * \param[in] function Function implementing the method.
     */
    template <typename InputFunction>
    FunctionalMethod(messages::MethodNameView name, InputFunction&& function)
        : name_(name), function_(std::forward<InputFunction>(function)) {}

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
        return messages::MessageSerializer::serialize_successful_response(
            request.id(), invoke(request.parameters()));
    }

    /*!
     * \brief Notify this method.
     *
     * \param[in] request Request.
     */
    void notify(const messages::ParsedRequest& request) {
        invoke(request.parameters());
    }

private:
    /*!
     * \brief Invoke the function of this method.
     *
     * \param[in] parameters Parameters.
     * \return Return value.
     */
    decltype(auto) invoke(const messages::ParsedParameters& parameters) {
        return std::apply(
            function_, parameters.as<std::decay_t<Parameters>...>());
    }

    //! Method name.
    messages::MethodName name_;

    //! Function.
    std::decay_t<Function> function_;
};

/*!
 * \brief Create a method implemented by a function object.
 *
 * \tparam Signature Signature of the method.
 * \tparam Function Type of the function implementing the method.
 * \param[in] name Name of the method.
 * \param[in] function Function implementing the method.
 * \return Method.
 */
template <typename Signature, typename Function>
[[nodiscard]] inline std::unique_ptr<FunctionalMethod<Signature, Function>>
create_functional_method(messages::MethodNameView name, Function&& function) {
    return std::make_unique<
        FunctionalMethod<Signature, std::decay_t<Function>>>(
        name, std::forward<Function>(function));
}

}  // namespace msgpack_rpc::methods
