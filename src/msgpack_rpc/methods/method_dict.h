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
 * \brief Definition of MethodDict class.
 */
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/methods/i_method.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Class of dictionaries of methods.
 */
class MethodDict {
public:
    /*!
     * \brief Constructor.
     */
    explicit MethodDict() = default;

    /*!
     * \brief Append a method.
     *
     * \param[in] method Method.
     */
    void append(std::unique_ptr<IMethod> method) {
        const messages::MethodNameView method_name = method->name();
        const auto result =
            methods_.try_emplace(method_name, std::move(method));
        if (!result.second) {
            const auto message =
                fmt::format("Duplicate method name {}.", method_name);
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT, message);
        }
    }

    /*!
     * \brief Get the method with a name. Throws an exception when not found.
     *
     * \param[in] name Name.
     * \return Method.
     */
    [[nodiscard]] IMethod* get(messages::MethodNameView name) const {
        const auto iter = methods_.find(name);
        if (iter == methods_.end()) {
            const auto message = fmt::format("Method {} not found.", name);
            throw MsgpackRPCException(StatusCode::INVALID_MESSAGE, message);
        }
        return iter->second.get();
    }

private:
    //! Dictionary of methods.
    std::unordered_map<messages::MethodNameView, std::unique_ptr<IMethod>>
        methods_{};
};

}  // namespace msgpack_rpc::methods
