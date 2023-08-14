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
 * \brief Definition of ParametersSerializer class.
 */
#pragma once

#include <tuple>
#include <type_traits>

#include <__utility/integer_sequence.h>

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Interface of serializer of parameters.
 */
class IParametersSerializer {
public:
    /*!
     * \brief Create a serialized request data.
     *
     * \param[in] method_name Name of the method to call with the request.
     * \param[in] request_id Message ID of the request.
     * \return Serialized request data.
     */
    [[nodiscard]] virtual messages::SerializedMessage create_serialized_request(
        messages::MethodNameView method_name,
        messages::MessageID request_id) const = 0;

    /*!
     * \brief Create a serialized notification data.
     *
     * \param[in] method_name Name of the method to call with the notification.
     * \return Serialized notification data.
     */
    [[nodiscard]] virtual messages::SerializedMessage
    create_serialized_notification(
        messages::MethodNameView method_name) const = 0;

    IParametersSerializer(const IParametersSerializer&) = delete;
    IParametersSerializer& operator=(const IParametersSerializer&) = delete;

    /*!
     * \brief Move constructor.
     */
    IParametersSerializer(IParametersSerializer&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    IParametersSerializer& operator=(
        IParametersSerializer&&) noexcept = default;

protected:
    //! Constructor.
    IParametersSerializer() noexcept = default;

    //! Destructor.
    ~IParametersSerializer() noexcept = default;
};

/*!
 * \brief Class of serializer of parameters.
 *
 * \tparam Parameters Types of parameters.
 */
template <typename... Parameters>
class ParametersSerializer final : public IParametersSerializer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] parameters Parameters.
     */
    explicit ParametersSerializer(const Parameters&... parameters)
        : parameters_(parameters...) {}

    //! \copydoc msgpack_rpc::clients::impl::IParametersSerializer::create_serialized_request
    [[nodiscard]] messages::SerializedMessage create_serialized_request(
        messages::MethodNameView method_name,
        messages::MessageID request_id) const override {
        return create_serialized_request_impl(
            method_name, request_id, std::index_sequence_for<Parameters...>());
    }

    //! \copydoc msgpack_rpc::clients::impl::IParametersSerializer::create_serialized_notification
    [[nodiscard]] messages::SerializedMessage create_serialized_notification(
        messages::MethodNameView method_name) const override {
        return create_serialized_notification_impl(
            method_name, std::index_sequence_for<Parameters...>());
    }

    ParametersSerializer(const ParametersSerializer&) = delete;
    ParametersSerializer& operator=(const ParametersSerializer&) = delete;

    /*!
     * \brief Move constructor.
     */
    ParametersSerializer(ParametersSerializer&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    ParametersSerializer& operator=(ParametersSerializer&&) noexcept = default;

    //! Destructor.
    ~ParametersSerializer() = default;

private:
    /*!
     * \brief Create a serialized request data.
     *
     * \tparam Indices Sequential indices of parameters.
     * \param[in] method_name Name of the method to call with the request.
     * \param[in] request_id Message ID of the request.
     * \return Serialized request data.
     */
    template <std::size_t... Indices>
    [[nodiscard]] messages::SerializedMessage create_serialized_request_impl(
        messages::MethodNameView method_name, messages::MessageID request_id,
        std::index_sequence<Indices...> /*indices*/) const {
        return messages::MessageSerializer::serialize_request(
            method_name, request_id, std::get<Indices>(parameters_)...);
    }

    /*!
     * \brief Create a serialized notification data.
     *
     * \tparam Indices Sequential indices of parameters.
     * \param[in] method_name Name of the method to call with the notification.
     * \return Serialized notification data.
     */
    template <std::size_t... Indices>
    [[nodiscard]] messages::SerializedMessage
    create_serialized_notification_impl(messages::MethodNameView method_name,
        std::index_sequence<Indices...> /*indices*/) const {
        return messages::MessageSerializer::serialize_notification(
            method_name, std::get<Indices>(parameters_)...);
    }

    //! Parameters.
    std::tuple<const Parameters&...> parameters_;
};

/*!
 * \brief Create ParametersSerializer object.
 *
 * \tparam Parameters Types of parameters.
 * \param[in] parameters Parameters.
 * \return ParametersSerializer object.
 */
template <typename... Parameters>
[[nodiscard]] ParametersSerializer<Parameters...> make_parameters_serializer(
    const Parameters&... parameters) {
    return ParametersSerializer<Parameters...>(parameters...);
}

}  // namespace msgpack_rpc::clients::impl
