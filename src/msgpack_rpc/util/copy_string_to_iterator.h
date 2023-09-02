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
 * \brief Definition of copy_string_to_iterator function.
 */
#pragma once

#include <algorithm>
#include <string_view>

namespace msgpack_rpc::util {

/*!
 * \brief Copy a string to a range of an iterator.
 *
 * \tparam Iterator Type of the iterator of the output range.
 * \param[in] string String to copy from.
 * \param[in] iterator Iterator of the range to copy to.
 * \return Iterator after the copy.
 */
template <typename Iterator>
inline Iterator copy_string_to_iterator(
    std::string_view string, Iterator iterator) {
    return std::copy(string.begin(), string.end(), iterator);
}

}  // namespace msgpack_rpc::util
