#include "msgpack_rpc/clients/client.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <string_view>
#include <thread>

#include "msgpack_rpc/clients/client_builder.h"

std::atomic<bool> is_stopped{false};

extern "C" void on_signal(int /*signal*/) { is_stopped = true; }

int main() {
    (void)std::signal(SIGINT, on_signal);
    (void)std::signal(SIGTERM, on_signal);

    constexpr auto server_uri = std::string_view("tcp://localhost:12345");

    auto client =
        msgpack_rpc::clients::ClientBuilder().connect_to(server_uri).build();
    client.start();

    while (!is_stopped) {
        try {
            client.notify("test");
        } catch (...) {
            //
        }
        // NOLINTNEXTLINE
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
