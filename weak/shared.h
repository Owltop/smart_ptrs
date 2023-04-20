#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <array>
#include <stdexcept>

// https://en.cppreference.com/w/cpp/memory/shared_ptr

class BaseBlock {
public:
    BaseBlock(){};
    virtual ~BaseBlock(){};
    virtual void Clear(){};
    size_t strong_counter = 0;
    size_t weak_counter = 0;
};

template <typename T>
class HolderBlock : public BaseBlock {
public:
    HolderBlock() {
        ++strong_counter;
        new (&storage) T();
    }

    template <typename... Args>
    HolderBlock(Args&&... args) {
        ++strong_counter;
        new (&storage) T(std::forward<Args>(args)...);
        // new (storage) T(std::forward<Args>(args)...);
    };

    T* GetPointer() {
        return reinterpret_cast<T*>(&storage);
    }

    void Clear() override {
        GetPointer()->~T();
    }

    ~HolderBlock() override {
        // GetPointer()->~T();
    }

protected:
    // T* tp = new (alignof(T)) T;
    alignas(T) std::array<std::byte, sizeof(T)> storage;
};

template <typename T>
class PointerBlock : public BaseBlock {
public:
    PointerBlock(T* obj_pointer) {
        ++strong_counter;
        object_pointer = obj_pointer;
    }

    void Clear() override {
        delete object_pointer;
        object_pointer = nullptr;
    }

    ~PointerBlock() override {
        delete object_pointer;
        object_pointer = nullptr;
    }

protected:
    T* object_pointer;
};

template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() {
        block = nullptr;
        real_object = nullptr;
    };

    SharedPtr(std::nullptr_t) {
        block = nullptr;
        real_object = nullptr;
    };

    explicit SharedPtr(T* ptr) {
        block = new PointerBlock(ptr);  //
        real_object = ptr;
        block->strong_counter = 1;
    };

    template <class U>
    explicit SharedPtr(U* ptr) {
        block = new PointerBlock(ptr);  //
        real_object = ptr;
        block->strong_counter = 1;
    };

    SharedPtr(const SharedPtr& other) {
        if (other.block != nullptr) {
            block = other.block;
            real_object = other.real_object;
            ++block->strong_counter;
        } else {
            block = nullptr;
            real_object = nullptr;
        }
    };

    template <class U>
    SharedPtr(const SharedPtr<U>& other) {
        if (other.block != nullptr) {
            block = other.block;
            real_object = other.real_object;
            ++block->strong_counter;
        } else {
            block = nullptr;
            real_object = nullptr;
        }
    };

    SharedPtr& operator=(const SharedPtr& other) {
        Reset();
        if (other.block != nullptr) {
            block = other.block;
            real_object = other.real_object;
            ++block->strong_counter;
        } else {
            block = nullptr;
            real_object = nullptr;
        }
        return *this;
    }

    template <class U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        Reset();
        if (other.block != nullptr) {
            block = other.block;
            real_object = other.real_object;
            ++block->strong_counter;
        } else {
            block = nullptr;
            real_object = nullptr;
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) {
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
    };

    template <class U>
    SharedPtr(SharedPtr<U>&& other) {
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
    };

    SharedPtr& operator=(SharedPtr&& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
        return *this;
    }

    template <class U>
    SharedPtr& operator=(SharedPtr<U>&& other) {
        Reset();
        block = other.block;
        real_object = other.real_object;
        other.block = nullptr;
        other.real_object = nullptr;
        return *this;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        block = other.block;
        real_object = ptr;
        ++block->strong_counter;
    };

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.block != nullptr && other.block->strong_counter == 0) {
            throw BadWeakPtr{};
        }
        block = other.block;
        real_object = other.real_object;
        if (block != nullptr) {
            ++block->strong_counter;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block == nullptr) {
            return;
        }
        if (block->strong_counter > 1) {
            --block->strong_counter;
            block = nullptr;
            real_object = nullptr;
        } else {
            if (block->weak_counter == 0) {
                block->Clear();
                delete block;
                block = nullptr;
                real_object = nullptr;
            } else {
                --block->strong_counter;
                block->Clear();
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
        if (block->strong_counter > 1) {
            --block->strong_counter;
            block = nullptr;
            real_object = nullptr;
        } else {
            if (block->weak_counter == 0) {
                delete block;
                block = nullptr;
                real_object = nullptr;
            } else {
                --block->strong_counter;
                block->Clear();
                block = nullptr;
                real_object = nullptr;
            }
        }
    };
    void Reset(T* ptr) {
        Reset();
        block = new PointerBlock(ptr);
        real_object = ptr;
        block->strong_counter = 1;
    };

    template <typename U>
    void Reset(U* ptr) {
        Reset();
        block = new PointerBlock(ptr);
        real_object = ptr;
        block->strong_counter = 1;
    };

    void Swap(SharedPtr& other) {
        std::swap(block, other.block);
        std::swap(real_object, other.real_object);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return real_object;
    };
    T& operator*() const {
        return *real_object;
    };

    T* operator->() const {
        return real_object;
    };

    size_t UseCount() const {
        if (block != nullptr) {
            return block->strong_counter;
        }
        return 0;
    };
    explicit operator bool() const {
        return real_object != nullptr;
    };

    template <typename TT, typename... Args>
    friend SharedPtr<TT> MakeShared(Args&&... args);

    BaseBlock* block;
    T* real_object;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    HolderBlock<T>* block = new HolderBlock<T>(std::forward<Args>(args)...);
    SharedPtr<T> ans;
    ans.block = block;
    block->strong_counter = 1;
    ans.real_object = block->GetPointer();
    return ans;
};

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
