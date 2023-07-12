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
 * \brief Definition of LogLevel enumeration.
 */
#pragma once

namespace msgpack_rpc::logging {

/*!
 * \brief Enumeration of log levels.
 */
enum class LogLevel {
    //! Trace. (Internal operations to send and receive messages.)
    TRACE,

    //! Debug. (Log of messages, initialization and finalization of clients and servers.)
    DEBUG,

    //! Information. (Not used in this library.)
    INFO,

    //! Warnings. (Unexpected conditions which don't stop operations.)
    WARN,

    //! Error. (Unexpected conditions which stop some operations in communication.)
    ERROR,

    //! Critical. (Unexpected conditions which can stop all operations in communication.)
    CRITICAL
};

}  // namespace msgpack_rpc::logging
