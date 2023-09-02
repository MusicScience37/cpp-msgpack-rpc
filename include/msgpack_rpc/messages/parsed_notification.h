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
 * \brief Definition of ParsedNotification class.
 */
#pragma once

#include <utility>

#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_parameters.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of parsed notification.
 */
class ParsedNotification {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] method_name Method name.
     * \param[in] parameters Parameters.
     */
    ParsedNotification(MethodNameView method_name, ParsedParameters parameters)
        : method_name_(method_name), parameters_(std::move(parameters)) {}

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] MethodNameView method_name() const noexcept {
        return method_name_;
    }

    /*!
     * \brief Get the parameters.
     *
     * \return Parameters.
     */
    [[nodiscard]] const ParsedParameters& parameters() const noexcept {
        return parameters_;
    }

private:
    //! Method name.
    MethodNameView method_name_;

    //! Parameters.
    ParsedParameters parameters_;
};

}  // namespace msgpack_rpc::messages
