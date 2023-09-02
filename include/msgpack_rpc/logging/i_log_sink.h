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
 * \brief Definition of ILogSink class.
 */
#pragma once

#include <string_view>

#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/source_location_view.h"

namespace msgpack_rpc::logging {

/*!
 * \brief Interface of log sinks to write logs.
 */
class ILogSink {
public:
    /*!
     * \brief Write a log.
     *
     * \param[in] location Location in source codes.
     * \param[in] level Log level.
     * \param[in] body Body of log.
     */
    virtual void write(
        SourceLocationView location, LogLevel level, std::string_view body) = 0;

    ILogSink(const ILogSink&) = delete;
    ILogSink(ILogSink&&) = delete;
    ILogSink& operator=(const ILogSink&) = delete;
    ILogSink& operator=(ILogSink&&) = delete;

    //! Destructor.
    virtual ~ILogSink() noexcept = default;

protected:
    //! Constructor.
    ILogSink() noexcept = default;
};

}  // namespace msgpack_rpc::logging
