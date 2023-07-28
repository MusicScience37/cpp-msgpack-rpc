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
 * \brief Definition of Server class.
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/transport/i_acceptor.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class of servers.
 */
class Server final : public IServer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] acceptors Acceptors.
     * \param[in] processor Processor of methods.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    Server(std::vector<std::shared_ptr<transport::IAcceptor>> acceptors,
        std::unique_ptr<methods::IMethodProcessor> processor,
        std::shared_ptr<executors::IExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : acceptors_(std::move(acceptors)),
          processor_(std::move(processor)),
          executor_(std::move(executor)),
          logger_(std::move(logger)) {}

    //! Destructor.
    ~Server() override = default;

    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

private:
    //! Acceptors.
    std::vector<std::shared_ptr<transport::IAcceptor>> acceptors_;

    //! Processor of methods.
    std::shared_ptr<methods::IMethodProcessor> processor_;

    //! Executor.
    std::shared_ptr<executors::IExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::servers
