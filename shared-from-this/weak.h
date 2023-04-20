#pragma once

#include "sw_fwd.h"  // Forward declaration
#include "shared.h"

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
        block = nullptr;
        real_object = nullptr;
    };

    WeakPtr(const WeakPtr& other) {
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->weak_counter;
        }
    };

    template <class U>
    WeakPtr(const WeakPtr<U>& other) {
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->weak_counter;
        }
    };

    WeakPtr(WeakPtr&& other) {
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
    };

    template <class U>
    WeakPtr(WeakPtr<U>&& other) {
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
    };

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->weak_counter;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-

    WeakPtr& operator=(const WeakPtr& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->weak_counter;
        }
        return *this;
    };

    template <class U>
    WeakPtr& operator=(const WeakPtr<U>& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->weak_counter;
        }
        return *this;
    };

    WeakPtr& operator=(WeakPtr&& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
        return *this;
    };

    template <class U>
    WeakPtr& operator=(WeakPtr<U>&& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block == nullptr) {
            return;
        }
        if (block->weak_counter > 1) {
            --block->weak_counter;
            block = nullptr;
            real_object = nullptr;
        } else {
            if (block->strong_counter == 0) {
                delete block;
                block = nullptr;
                real_object = nullptr;
            } else {
                --block->weak_counter;
                block = nullptr;
                real_object = nullptr;
            }
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block == nullptr) {
            return;
        }
        if (block->weak_counter > 1) {
            --block->weak_counter;
            block = nullptr;
            real_object = nullptr;
        } else {
            if (block->strong_counter == 0) {
                delete block;
                block = nullptr;
                real_object = nullptr;
            } else {
                --block->weak_counter;
                block = nullptr;
                real_object = nullptr;
            }
        }
    };
    void Swap(WeakPtr& other) {
        std::swap(block, other.block);
        std::swap(real_object, other.real_object);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (block == nullptr) {
            return 0;
        }
        return block->strong_counter;
    };

    bool Expired() const {
        if (block == nullptr) {
            return true;
        }
        if (block->strong_counter == 0) {
            return true;
        }
        return false;
    };

    SharedPtr<T> Lock() const {
        SharedPtr<T> ans;
        if (block == nullptr) {
            ans.block = nullptr;
            ans.real_object = nullptr;
        } else if (block->strong_counter == 0) {
            ans.block = nullptr;
            ans.real_object = nullptr;
        } else {
            ans.block = block;
            ans.real_object = real_object;
            //--block->weak_counter;
            ++block->strong_counter;
        }
        return ans;
    };

    BaseBlock* block;
    T* real_object;
};
