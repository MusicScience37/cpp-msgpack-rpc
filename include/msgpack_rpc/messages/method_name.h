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
 * \brief Definition of MethodName class.
 */
#pragma once

#include <string>
#include <string_view>

namespace msgpack_rpc::messages {

/*!
 * \brief Class of method names.
 */
class MethodName {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Method name.
     */
    explicit MethodName(std::string_view name) noexcept : name_(name) {
        // TODO Add a validation of UTF-8.
    }

    /*!
     * \brief Get the method name.
     *
     * \return Method name.
     */
    [[nodiscard]] std::string_view name() const noexcept { return name_; }

private:
    //! Method name.
    std::string name_;
};

}  // namespace msgpack_rpc::messages
