/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of classes for shared objects.
 */
#pragma once

#include <atomic>
#include <memory>
#include <new>
#include <optional>
#include <utility>
#include <vector>

namespace msgpack_rpc::util::impl {

/*!
 * \brief Class of buffers of shared objects.
 *
 * \tparam T Type of the shared objects.
 */
template <typename T>
class SharedObjectBuffer {
public:
    /*!
     * \brief Constructor.
     */
    SharedObjectBuffer() noexcept = default;

    /*!
     * \brief Try to start to use.
     *
     * \retval true Succeeded.
     * \retval false Failed.
     */
    [[nodiscard]] bool try_start_use() noexcept {
        int empty = -1;
        constexpr int initial = 1;
        return reference_count_.compare_exchange_strong(empty, initial,
            std::memory_order_acquire, std::memory_order_relaxed);
    }

    /*!
     * \brief Construct an object.
     *
     * \warning This can be called safely only when the reference count is 1.
     *
     * \tparam Args Types of arguments.
     * \param[in] args Arguments.
     */
    template <typename... Args>
    void emplace(Args&&... args) {
        buffer_.emplace(std::forward<Args>(args)...);
    }

    /*!
     * \brief Increase the reference count.
     */
    void add_reference() noexcept {
        reference_count_.fetch_add(1, std::memory_order_relaxed);
    }

    /*!
     * \brief Decrease the reference count and destruct the object if needed.
     */
    void release() noexcept {
        const int reference_count =
            reference_count_.fetch_sub(1, std::memory_order_acq_rel);
        if (reference_count == 1) {
            buffer_.reset();
            const int empty = -1;
            reference_count_.store(empty, std::memory_order_release);
        }
    }

    /*!
     * \brief Access the object.
     *
     * \return Pointer to the object.
     */
    [[nodiscard]] T* get() noexcept {
        return &(*buffer_);  // NOLINT
    }

private:
    //! Reference count. (-1 for unused buffer.)
    std::atomic<int> reference_count_{-1};

    //! Buffer.
    std::optional<T> buffer_{};
};

template <typename T>
class SharedObjectMemoryPool;

/*!
 * \brief Class to hold shared objects.
 *
 * \tparam T Types of the shared objects.
 */
template <typename T>
class SharedObject {
public:
    /*!
     * \brief Constructor.
     */
    SharedObject() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] buffer Buffer of a shared object.
     * \param[in] memory_pool Memory pool.
     */
    explicit SharedObject(SharedObjectBuffer<T>* buffer,
        std::shared_ptr<SharedObjectMemoryPool<T>> memory_pool) noexcept
        : buffer_(buffer), memory_pool_(std::move(memory_pool)) {}

    /*!
     * \brief Copy constructor.
     *
     * \param[in] other Object to move from.
     */
    SharedObject(const SharedObject& other) noexcept
        : buffer_(other.buffer_), memory_pool_(other.memory_pool_) {
        if (buffer_ != nullptr) {
            buffer_->add_reference();
        }
    }

    /*!
     * \brief Move constructor.
     *
     * \param[in] other Object to move from.
     */
    SharedObject(SharedObject&& other) noexcept
        : buffer_(std::exchange(other.buffer_, nullptr)),
          memory_pool_(std::exchange(other.memory_pool_,
              std::shared_ptr<SharedObjectMemoryPool<T>>())) {}

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] other Object to copy from.
     * \return This object.
     */
    SharedObject& operator=(const SharedObject& other) noexcept {
        if (this == &other) {
            return *this;
        }
        reset();
        buffer_ = other.buffer_;
        memory_pool_ = other.memory_pool_;
        if (buffer_ != nullptr) {
            buffer_->add_reference();
        }
        return *this;
    }

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] other Object to move from.
     * \return This object.
     */
    SharedObject& operator=(SharedObject&& other) noexcept {
        reset();
        buffer_ = std::exchange(other.buffer_, nullptr);
        memory_pool_ = std::exchange(
            other.memory_pool_, std::shared_ptr<SharedObjectMemoryPool<T>>());
        return *this;
    }

    /*!
     * \brief Destructor.
     */
    ~SharedObject() noexcept { reset(); }

    /*!
     * \brief Remove the buffer.
     */
    void reset() noexcept {
        if (buffer_ != nullptr) {
            buffer_->release();
            buffer_ = nullptr;
            memory_pool_.reset();
        }
    }

private:
    //! Buffer of the shared object.
    SharedObjectBuffer<T>* buffer_{nullptr};

    //! Memory pool.
    std::shared_ptr<SharedObjectMemoryPool<T>> memory_pool_{};
};

/*!
 * \brief Class of memory pools of shared objects.
 *
 * \tparam T Type of shared objects.
 */
template <typename T>
class SharedObjectMemoryPool
    : public std::enable_shared_from_this<SharedObjectMemoryPool<T>> {
public:
    /*!
     * \brief Create a memory pool.
     *
     * \param[in] num_buffers Number of buffers.
     * \return Created memory pool.
     */
    static std::shared_ptr<SharedObjectMemoryPool<T>> create(
        std::size_t num_buffers) {
        return std::make_shared<SharedObjectMemoryPool<T>>(num_buffers);
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] num_buffers Number of buffers.
     */
    explicit SharedObjectMemoryPool(std::size_t num_buffers)
        : buffers_(num_buffers) {}

    SharedObjectMemoryPool(const SharedObjectMemoryPool&) = delete;
    SharedObjectMemoryPool(SharedObjectMemoryPool&&) = delete;
    SharedObjectMemoryPool& operator=(const SharedObjectMemoryPool&) = delete;
    SharedObjectMemoryPool& operator=(SharedObjectMemoryPool&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~SharedObjectMemoryPool() = default;

    /*!
     * \brief Create a shared object.
     *
     * \tparam Args Types of arguments.
     * \param[in] args Arguments.
     * \return Created object.
     */
    template <typename... Args>
    [[nodiscard]] SharedObject<T> create(Args&&... args) {
        const std::size_t initial_buffer_index = next_buffer_index_;
        while (true) {
            const std::size_t buffer_index = next_buffer_index_++;
            next_buffer_index_ %= buffers_.size();
            SharedObjectBuffer<T>& buffer = buffers_[buffer_index];
            if (!buffer.try_start_use()) {
                if (next_buffer_index_ == initial_buffer_index) {
                    throw std::bad_alloc();
                }
                continue;
            }

            try {
                buffer.emplace(std::forward<Args>(args)...);
                return SharedObject<T>(&buffer, this->shared_from_this());
            } catch (...) {
                buffer.release();
                throw;
            }
        }
    }

private:
    //! Buffers.
    std::vector<SharedObjectBuffer<T>> buffers_;

    //! Index of the next buffer.
    std::size_t next_buffer_index_{0};
};

}  // namespace msgpack_rpc::util::impl
