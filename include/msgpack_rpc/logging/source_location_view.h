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
 * \brief Definition of SourceLocationView class.
 */
#pragma once

#include <cstdint>
#include <string_view>

namespace msgpack_rpc::logging {

/*!
 * \brief Class of locations in source codes.
 */
class SourceLocationView {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] filepath Filepath.
     * \param[in] line Line number.
     * \param[in] function Function name.
     *
     * \note This class doesn't manage memory of filepaths.
     */
    constexpr SourceLocationView(std::string_view filepath, std::uint32_t line,
        std::string_view function) noexcept
        : filepath_(filepath), line_(line), function_(function) {}

    /*!
     * \brief Get the filepath.
     *
     * \return Filepath.
     */
    [[nodiscard]] constexpr std::string_view filepath() const noexcept {
        return filepath_;
    }

    /*!
     * \brief Get the line number.
     *
     * \return Line number.
     */
    [[nodiscard]] constexpr std::uint32_t line() const noexcept {
        return line_;
    }

    /*!
     * \brief Get the function name.
     *
     * \return Function name.
     */
    [[nodiscard]] constexpr std::string_view function() const noexcept {
        return function_;
    }

private:
    //! Filepath.
    std::string_view filepath_;

    //! Line number.
    std::uint32_t line_;

    //! Function name.
    std::string_view function_;
};

}  // namespace msgpack_rpc::logging

//! Macro to get the current filepath.
#define MSGPACK_RPC_CURRENT_FILEPATH __FILE__

//! Macro to get the current line number.
#define MSGPACK_RPC_CURRENT_LINE __LINE__

#ifdef MSGPACK_RPC_DOCUMENTATION
/*!
 * \brief Macro to get the current function name.
 *
 * \note This macro depends on compilers.
 */
#define MSGPACK_RPC_CURRENT_FUNCTION <implementation defined strings>
#elif __GNUC__  // GCC and Clang
#define MSGPACK_RPC_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif _MSC_VER  // MSVC
#define MSGPACK_RPC_CURRENT_FUNCTION __FUNCSIG__
#else  // fallback (C++ standard)
#define MSGPACK_RPC_CURRENT_FUNCTION __func__
#endif

//! Macro to get the current location in source codes.
#define MSGPACK_RPC_CURRENT_SOURCE_LOCATION()                      \
    ::msgpack_rpc::logging::SourceLocationView(                    \
        std::string_view(MSGPACK_RPC_CURRENT_FILEPATH), /*NOLINT*/ \
        static_cast<std::uint32_t>(MSGPACK_RPC_CURRENT_LINE),      \
        std::string_view(MSGPACK_RPC_CURRENT_FUNCTION)) /*NOLINT*/
