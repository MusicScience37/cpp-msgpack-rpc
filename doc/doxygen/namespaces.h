/*
 * Copyright 2023 Kenta Kabashima.
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
 * \brief Documentation of namespaces
 */

/*!
 * \dir include
 * \brief Directory of public headers.
 */

/*!
 * \dir src
 * \brief Directory of source codes.
 */

/*!
 * \dir include/msgpack_rpc
 * \brief Directory of public headers of cpp-msgpack-rpc library.
 */

/*!
 * \dir src/msgpack_rpc
 * \brief Directory of source codes of cpp-msgpack-rpc library.
 */

/*!
 * \brief Namespace of cpp-msgpack-rpc library.
 */
namespace msgpack_rpc {

    /*!
     * \dir include/msgpack_rpc/addresses
     * \brief Directory of public headers of addresses.
     */

    /*!
     * \dir src/msgpack_rpc/addresses
     * \brief Directory of source codes of addresses.
     */

    /*!
     * \brief Namespace of addresses.
     */
    namespace addresses {}

    /*!
     * \dir include/msgpack_rpc/clients
     * \brief Directory of public headers of clients.
     */

    /*!
     * \dir src/msgpack_rpc/clients
     * \brief Directory of source codes of clients.
     */

    /*!
     * \brief Namespace of clients.
     */
    namespace clients {

        /*!
         * \dir include/msgpack_rpc/clients/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \dir src/msgpack_rpc/clients/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \brief Namespace of internal implementations.
         */
        namespace impl {}

    }  // namespace clients

    /*!
     * \dir include/msgpack_rpc/common
     * \brief Directory of public headers of common classes and functions.
     */

    /*!
     * \dir src/msgpack_rpc/common
     * \brief Directory of source codes of common classes and functions.
     */

    /*!
     * \brief Namespace of common classes and functions.
     */
    inline namespace common {}

    /*!
     * \dir include/msgpack_rpc/config
     * \brief Directory of public headers of configurations.
     */

    /*!
     * \dir src/msgpack_rpc/config
     * \brief Directory of source codes of configurations.
     */

    /*!
     * \brief Namespace of configurations.
     */
    namespace config {}

    /*!
     * \dir include/msgpack_rpc/executors
     * \brief Directory of public headers of executors to process asynchronous
     * tasks.
     */

    /*!
     * \dir src/msgpack_rpc/executors
     * \brief Directory of source codes of executors to process asynchronous
     * tasks.
     */

    /*!
     * \brief Namespace of executors to process asynchronous tasks.
     */
    namespace executors {}

    /*!
     * \dir include/msgpack_rpc/logging
     * \brief Directory of public headers of logging.
     */

    /*!
     * \dir src/msgpack_rpc/logging
     * \brief Directory of source codes of logging.
     */

    /*!
     * \brief Namespace of logging.
     */
    namespace logging {

        /*!
         * \dir src/msgpack_rpc/logging/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \brief Namespace of internal implementations.
         */
        namespace impl {

            /*!
             * \brief Namespace of internal implementation of logging using
             * spdlog library.
             */
            namespace spdlog_backend {}

        }  // namespace impl

    }  // namespace logging

    /*!
     * \dir include/msgpack_rpc/messages
     * \brief Directory of public headers of messages.
     */

    /*!
     * \dir src/msgpack_rpc/messages
     * \brief Directory of source codes of messages.
     */

    /*!
     * \brief Namespace of messages.
     */
    namespace messages {

        /*!
         * \dir src/msgpack_rpc/messages/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \brief Namespace of internal implementations.
         */
        namespace impl {}

    }  // namespace messages

    /*!
     * \dir include/msgpack_rpc/methods
     * \brief Directory of public headers of methods in MessagePack-RPC.
     */

    /*!
     * \dir src/msgpack_rpc/methods
     * \brief Directory of source codes of methods in MessagePack-RPC.
     */

    /*!
     * \brief Namespace of methods in MessagePack-RPC.
     */
    namespace methods {}

    /*!
     * \dir include/msgpack_rpc/servers
     * \brief Directory of public headers of servers.
     */

    /*!
     * \dir src/msgpack_rpc/servers
     * \brief Directory of source codes of servers.
     */

    /*!
     * \brief Namespace of servers.
     */
    namespace servers {

        /*!
         * \dir include/msgpack_rpc/servers/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \dir src/msgpack_rpc/servers/impl
         * \brief Directory of internal implementation.
         */

        /*!
         * \brief Namespace of internal implementation.
         */
        namespace impl {}

    }  // namespace servers

    /*!
     * \dir include/msgpack_rpc/transport
     * \brief Directory of public headers of transport of messages.
     */

    /*!
     * \dir src/msgpack_rpc/transport
     * \brief Directory of source codes of transport of messages.
     */

    /*!
     * \brief Namespace of transport of messages.
     */
    namespace transport {

        /*!
         * \brief Namespace of internal implementations.
         */
        namespace impl {}

        /*!
         * \dir src/msgpack_rpc/transport/tcp
         * \brief Directory of source codes of transport of messages via TCP.
         */

        /*!
         * \brief Namespace of transport of messages via TCP.
         */
        namespace tcp {}

    }  // namespace transport

}  // namespace msgpack_rpc

/*!
 * \brief Namespace of fmt library.
 */
namespace fmt {}
