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
 * \brief Common definitions.
 */
#pragma once

#include <string_view>

namespace msgpack_rpc_test {

/*!
 * \brief URI of servers for tests.
 *
 * \note THe port number was selected randomly.
 */
constexpr std::string_view SERVER_URL = "tcp://localhost:18612";

}  // namespace msgpack_rpc_test
