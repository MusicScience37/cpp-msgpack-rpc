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
 * \brief Definition of ParsedParameters class.
 */
#pragma once

#include <memory>
#include <string_view>
#include <tuple>
#include <utility>

#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of parsed parameters.
 */
class ParsedParameters {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] object Object of parameters in msgpack library.
     * \param[in] zone Zone in msgpack library.
     */
    explicit ParsedParameters(
        msgpack::object object, std::shared_ptr<msgpack::zone> zone)
        : object_(object), zone_(std::move(zone)) {
        if (object.type != msgpack::type::ARRAY) {
            throw MsgpackRPCException(
                StatusCode::INVALID_MESSAGE, "Invalid type of parameters.");
        }
    }

    /*!
     * \brief Get parameters as given types.
     *
     * \tparam Parameters Type of parameters.
     * \return Parameters.
     */
    template <typename... Parameters>
    [[nodiscard]] std::tuple<Parameters...> as() const {
        try {
            // msgpack library doesn't check the number of elements in tuples.
            if (object_.via.array.size != sizeof...(Parameters)) {
                throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
                    "Invalid number of parameters.");
            }
            return object_.as<std::tuple<Parameters...>>();
        } catch (const msgpack::type_error&) {
            throw MsgpackRPCException(
                StatusCode::TYPE_ERROR, "Invalid types of parameters.");
        }
    }

private:
    //! Object of parameters in msgpack library.
    msgpack::object object_;

    //! Zone in msgpack library.
    std::shared_ptr<msgpack::zone> zone_;
};

}  // namespace msgpack_rpc::messages
