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
 * \brief Declaration of create_test_logger function.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc_test {

[[nodiscard]] std::shared_ptr<msgpack_rpc::logging::Logger>
create_test_logger();

}  // namespace msgpack_rpc_test
