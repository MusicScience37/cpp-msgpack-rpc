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
 * \brief Declaration of check_connectivity function.
 */
#pragma once

#include <vector>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"

/*!
 * \brief Check whether an address is accepting connections.
 *
 * \param[in] address Address.
 */
void check_connectivity(const msgpack_rpc::addresses::TCPAddress& address);

/*!
 * \brief Check whether a URI is accepting connections.
 *
 * \param[in] uri URI.
 */
void check_connectivity(const msgpack_rpc::addresses::URI& uri);

/*!
 * \brief Check whether URIs are accepting connections.
 *
 * \param[in] uris URIs.
 */
void check_connectivity(const std::vector<msgpack_rpc::addresses::URI>& uris);
