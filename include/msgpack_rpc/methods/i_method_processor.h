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
 * \brief Definition of IMethodProcessor class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/i_method.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Interface of processor of method calls.
 */
class IMethodProcessor {
public:
    /*!
     * \brief Append a method.
     *
     * \param[in] method Method.
     */
    virtual void append(std::unique_ptr<IMethod> method) = 0;

    /*!
     * \brief Call a method.
     *
     * \param[in] request Request.
     * \return Serialized response.
     */
    [[nodiscard]] virtual messages::SerializedMessage call(
        const messages::ParsedRequest& request) = 0;

    /*!
     * \brief Notify a method.
     *
     * \param[in] notification Notification.
     */
    virtual void notify(const messages::ParsedNotification& notification) = 0;

    IMethodProcessor(const IMethodProcessor&) = delete;
    IMethodProcessor(IMethodProcessor&&) = delete;
    IMethodProcessor& operator=(const IMethodProcessor&) = delete;
    IMethodProcessor& operator=(IMethodProcessor&&) = delete;

    //! Destructor.
    virtual ~IMethodProcessor() noexcept = default;

protected:
    //! Constructor.
    IMethodProcessor() noexcept = default;
};

}  // namespace msgpack_rpc::methods
