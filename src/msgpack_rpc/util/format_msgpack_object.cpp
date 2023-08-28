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
 * \brief Implementation of format_msgpack_object function.
 */
#include "msgpack_rpc/util/format_msgpack_object.h"

#include "msgpack_rpc/util/format_msgpack_object_to.h"

namespace msgpack_rpc::util::impl {

FmtMsgpackObjectProxy::FmtMsgpackObjectProxy(const msgpack::object& object)
    : object_(object) {}

const msgpack::object& FmtMsgpackObjectProxy::object() const noexcept {
    return object_;
}

}  // namespace msgpack_rpc::util::impl

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::util::impl::FmtMsgpackObjectProxy>::
    format(  // NOLINT(readability-convert-member-functions-to-static): required by an external library.
        const msgpack_rpc::util::impl::FmtMsgpackObjectProxy& value,
        format_context& context) const {
    return msgpack_rpc::util::format_msgpack_object_to(
        context.out(), value.object());
}

}  // namespace fmt

namespace msgpack_rpc::util {

impl::FmtMsgpackObjectProxy format_msgpack_object(
    const msgpack::object& object) {
    return impl::FmtMsgpackObjectProxy(object);
}

}  // namespace msgpack_rpc::util
