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
 * \brief Test of format_msgpack_object function.
 */
#include "msgpack_rpc/util/format_msgpack_object.h"

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <msgpack.hpp>

TEST_CASE("msgpack_rpc::util::format_msgpack_object") {
    using msgpack_rpc::util::format_msgpack_object;

    // TODO Format time.

    SECTION("format a nil") {
        const auto object = msgpack::object();

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "null");
    }

    SECTION("format true") {
        const auto object = msgpack::object(true);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "true");
    }

    SECTION("format false") {
        const auto object = msgpack::object(false);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "false");
    }

    SECTION("format a positive integer") {
        const auto object = msgpack::object(12345);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "12345");
    }

    SECTION("format zero") {
        const auto object = msgpack::object(0);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "0");
    }

    SECTION("format a negative integer") {
        const auto object = msgpack::object(-12345);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "-12345");
    }

    SECTION("format a floating-point number") {
        const auto object = msgpack::object(1.25);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == "1.25");
    }

    SECTION("format a string") {
        msgpack::zone zone;
        const auto object = msgpack::object("abc", zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"("abc")");
    }

    SECTION("format a binary") {
        msgpack::zone zone;
        const auto object = msgpack::object(
            std::vector<unsigned char>{static_cast<unsigned char>(0x00),
                static_cast<unsigned char>(0x06),
                static_cast<unsigned char>(0xA0),
                static_cast<unsigned char>(0xFF)},
            zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"(bin(0006A0FF))");
    }

    SECTION("format an empty array") {
        msgpack::zone zone;
        const auto object = msgpack::object(std::make_tuple(), zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"([])");
    }

    SECTION("format an array with one element") {
        msgpack::zone zone;
        const auto object = msgpack::object(std::make_tuple("abc"), zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"(["abc"])");
    }

    SECTION("format an array with two elements") {
        msgpack::zone zone;
        const auto object =
            msgpack::object(std::make_tuple(12345, "abc"), zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"([12345, "abc"])");
    }

    SECTION("format a nested array") {
        msgpack::zone zone;
        const auto object = msgpack::object(
            std::make_tuple(std::make_tuple("abc"), 12345), zone);

        const std::string result =
            fmt::format("{}", format_msgpack_object(object));

        CHECK(result == R"([["abc"], 12345])");
    }

    SECTION("format an empty map") {
        msgpack::sbuffer packer_buffer;
        msgpack::packer<msgpack::sbuffer> packer{packer_buffer};
        packer.pack_map(0);
        const auto object =
            msgpack::unpack(packer_buffer.data(), packer_buffer.size());

        const std::string result =
            fmt::format("{}", format_msgpack_object(*object));

        CHECK(result == R"({})");
    }

    SECTION("format a map with one element") {
        msgpack::sbuffer packer_buffer;
        msgpack::packer<msgpack::sbuffer> packer{packer_buffer};
        packer.pack_map(1);
        packer.pack(1);
        packer.pack("a");
        const auto object =
            msgpack::unpack(packer_buffer.data(), packer_buffer.size());

        const std::string result =
            fmt::format("{}", format_msgpack_object(*object));

        CHECK(result == R"({1: "a"})");
    }

    SECTION("format a map with two elements") {
        msgpack::sbuffer packer_buffer;
        msgpack::packer<msgpack::sbuffer> packer{packer_buffer};
        packer.pack_map(2);
        packer.pack(1);
        packer.pack("a");
        packer.pack("b");
        packer.pack(2);
        const auto object =
            msgpack::unpack(packer_buffer.data(), packer_buffer.size());

        const std::string result =
            fmt::format("{}", format_msgpack_object(*object));

        CHECK(result == R"({1: "a", "b": 2})");
    }

    SECTION("format an extension") {
        msgpack::sbuffer packer_buffer;
        msgpack::packer<msgpack::sbuffer> packer{packer_buffer};
        const auto ext_type = static_cast<std::uint8_t>(37);
        const auto ext_body = std::vector<unsigned char>{
            static_cast<unsigned char>(0x00), static_cast<unsigned char>(0x06),
            static_cast<unsigned char>(0xA0), static_cast<unsigned char>(0xFF)};
        packer.pack_ext(ext_body.size(), ext_type);
        packer.pack_ext_body(
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            reinterpret_cast<const char*>(ext_body.data()), ext_body.size());
        const auto object =
            msgpack::unpack(packer_buffer.data(), packer_buffer.size());

        const std::string result =
            fmt::format("{}", format_msgpack_object(*object));

        CHECK(result == R"(ext(37, 0006A0FF))");
    }
}
