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
 * \brief Definition of TCPAcceptor class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/transport/acceptor_impl.h"
#include "msgpack_rpc/transport/i_acceptor.h"

namespace msgpack_rpc::transport::tcp {

class TCPAcceptor final : public IAcceptor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] acceptor Acceptor.
     */
    explicit TCPAcceptor(std::shared_ptr<AcceptorImpl> acceptor);

    //! \copydoc msgpack_rpc::transport::IAcceptor::start
    void start(ConnectionCallback on_connection) override;

    //! \copydoc msgpack_rpc::transport::IAcceptor::stop
    void stop() override;

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::Address& local_address()
        const noexcept override;

    TCPAcceptor(const TCPAcceptor&) = delete;
    TCPAcceptor(TCPAcceptor&&) = delete;
    TCPAcceptor& operator=(const TCPAcceptor&) = delete;
    TCPAcceptor& operator=(TCPAcceptor&&) = delete;

    ~TCPAcceptor() override;

private:
    //! Acceptor.
    std::shared_ptr<AcceptorImpl> acceptor_;

    //! Address of the local endpoint.
    addresses::Address local_address_;
};

}  // namespace msgpack_rpc::transport::tcp
