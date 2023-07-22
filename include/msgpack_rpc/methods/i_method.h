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
 * \brief Definition of IMethod class.
 */
#pragma once

#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Interface of methods.
 */
class IMethod {
public:
    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] virtual messages::MethodNameView name() const noexcept = 0;

    /*!
     * \brief Call this method.
     *
     * \param[in] request Request.
     * \return Serialized response.
     */
    [[nodiscard]] virtual messages::SerializedMessage call(
        const messages::ParsedRequest& request) = 0;

    /*!
     * \brief Notify this method.
     *
     * \param[in] request Request.
     */
    virtual void notify(const messages::ParsedRequest& request) = 0;

    IMethod(const IMethod&) = delete;
    IMethod(IMethod&&) = delete;
    IMethod& operator=(const IMethod&) = delete;
    IMethod& operator=(IMethod&&) = delete;

    //! Destructor.
    virtual ~IMethod() noexcept = default;

protected:
    //! Constructor.
    IMethod() noexcept = default;
};

}  // namespace msgpack_rpc::methods
