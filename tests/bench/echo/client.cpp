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
 * \brief Implementation of clients for benchmarks.
 */
#include "msgpack_rpc/clients/client.h"

#include <cstdlib>
#include <string>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/do_not_optimize.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/param/parameter_value_vector.h>
#include <stat_bench/plot_option.h>

#include "common.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config.h"

class EchoFixture : public stat_bench::FixtureBase {
public:
    EchoFixture() {
        this->add_param<std::string>("type")
            ->add("TCPv4")
            ->add("TCPv6")
#if MSGPACK_RPC_HAS_UNIX_SOCKETS
            ->add("Unix")
#endif
            ;
        this->add_param<std::size_t>("size")
            ->add(0)
            ->add(1)
            ->add(32)    // NOLINT
            ->add(1024)  // NOLINT
#ifdef NDEBUG
            ->add(8 * 1024)     // NOLINT
            ->add(64 * 1024)    // NOLINT
            ->add(128 * 1024)   // NOLINT
            ->add(256 * 1024)   // NOLINT
            ->add(512 * 1024)   // NOLINT
            ->add(1024 * 1024)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        const auto server_type_str = context.get_param<std::string>("type");
        if (server_type_str == "TCPv4") {
            server_type_ = msgpack_rpc_test::ServerType::TCP4;
        } else if (server_type_str == "TCPv6") {
            server_type_ = msgpack_rpc_test::ServerType::TCP6;
        } else if (server_type_str == "Unix") {
            server_type_ = msgpack_rpc_test::ServerType::UNIX_SOCKET;
        } else {
            // This won't be executed unless a bug exists.
            std::abort();
        }

        auto command_client =
            msgpack_rpc::clients::ClientBuilder()
                .connect_to(msgpack_rpc_test::COMMAND_SERVER_URI)
                .build();
        server_uri_ = command_client.call<std::string>(
            "prepare", static_cast<int>(server_type_));

        data_size_ = context.get_param<std::size_t>("size");
    }

    [[nodiscard]] msgpack_rpc::clients::Client prepare_client() const {
        auto client = msgpack_rpc::clients::ClientBuilder()
                          .connect_to(server_uri_)
                          .build();
        return client;
    }

    [[nodiscard]] std::string create_data() const {
        return std::string(data_size_, 'a');
    }

private:
    //! Type of the server.
    msgpack_rpc_test::ServerType server_type_{};

    //! URI of the server.
    std::string server_uri_{};

    //! Size of the data.
    std::size_t data_size_{};
};

STAT_BENCH_GROUP("echo")
    .add_parameter_to_time_line_plot(
        "size", stat_bench::PlotOption::log_parameter)
    .add_parameter_to_time_violin_plot("size");

STAT_BENCH_CASE_F(EchoFixture, "echo", "echo") {
    auto client = prepare_client();

    const auto data = create_data();

    STAT_BENCH_MEASURE() {
        stat_bench::do_not_optimize(client.call<std::string>("echo", data));
    };
}

STAT_BENCH_MAIN
