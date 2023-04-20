#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
#include <exception>

struct Slug {};

// Primary template

template <typename T, typename Deleter = Slug>
class UniquePtr {
public:
    using Lref = std::add_lvalue_reference<T>;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        ptr_.GetFirst() = ptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
    }

    UniquePtr(T* ptr, Deleter deleter) {
        ptr_.GetFirst() = ptr;
        ptr_.GetSecond() = std::move(deleter);
    }

    template <class U, class Del>
    UniquePtr(UniquePtr<U, Del>&& other) noexcept {
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
    }

    UniquePtr(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class U, class Del>
    UniquePtr& operator=(UniquePtr<U, Del>&& other) noexcept {
        if (this->ptr_.GetFirst() == other.ptr_.GetFirst()) {
            return *this;
        }
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = nullptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;
        return old;
    };

    void Reset(T* ptr = nullptr) {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete old;
        } else {
            Deleter()(old);
        }
    };

    void Swap(UniquePtr& other) {
        std::swap(ptr_.GetFirst(), other.ptr_.GetFirst());
        std::swap(ptr_.GetSecond(), other.ptr_.GetSecond());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    };
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    };
    explicit operator bool() const {
        return ptr_.GetFirst() != nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *(ptr_.GetFirst());
    };

    T* operator->() const {
        return ptr_.GetFirst();
    };

    // protected:
    CompressedPair<T*, Deleter> ptr_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        ptr_.GetFirst() = ptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
    }

    UniquePtr(T* ptr, Deleter deleter) {
        ptr_.GetFirst() = ptr;
        ptr_.GetSecond() = std::move(deleter);
    }

    template <class U, class Del>
    UniquePtr(UniquePtr<U, Del>&& other) noexcept {
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
    }

    UniquePtr(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class U, class Del>
    UniquePtr& operator=(UniquePtr<U, Del>&& other) noexcept {
        if (this->ptr_.GetFirst() == other.ptr_.GetFirst()) {
            return *this;
        }
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete[] ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete[] ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = nullptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete[] ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;
        return old;
    };

    void Reset(T* ptr = nullptr) {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete[] old;
        } else {
            Deleter()(old);
        }
    };

    void Swap(UniquePtr& other) {
        std::swap(ptr_.GetFirst(), other.ptr_.GetFirst());
        std::swap(ptr_.GetSecond(), other.ptr_.GetSecond());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    };
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    };
    explicit operator bool() const {
        return ptr_.GetFirst() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator*() const {
        return *(ptr_.GetFirst());
    };
    T* operator->() const {
        return ptr_.GetFirst();
    };

    T& operator[](size_t i) const {
        size_t cur = 0;
        auto cur_p = ptr_.GetFirst();
        while (cur != i) {
            ++cur_p;
            ++cur;
        }
        return *cur_p;
    }

    T& operator[](size_t i) {
        size_t cur = 0;
        auto cur_p = ptr_.GetFirst();
        while (cur != i) {
            ++cur_p;
            ++cur;
        }
        return *cur_p;
    }

    // protected:
    CompressedPair<T*, Deleter> ptr_;
};

template <typename Deleter>
class UniquePtr<void, Deleter> {
public:
    using Lref = std::add_lvalue_reference<void>;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(void* ptr = nullptr) {
        ptr_.GetFirst() = ptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
    }

    UniquePtr(void* ptr, Deleter deleter) {
        ptr_.GetFirst() = ptr;
        ptr_.GetSecond() = std::move(deleter);
    }

    template <class U, class Del>
    UniquePtr(UniquePtr<U, Del>&& other) noexcept {
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
    }

    UniquePtr(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class U, class Del>
    UniquePtr& operator=(UniquePtr<U, Del>&& other) noexcept {
        if (this->ptr_.GetFirst() == other.ptr_.GetFirst()) {
            return *this;
        }
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = other.ptr_.GetFirst();
        ptr_.GetSecond() = std::move(other.ptr_.GetSecond());
        other.ptr_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
        ptr_.GetFirst() = nullptr;
        Deleter del;
        ptr_.GetSecond() = std::move(del);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete ptr_.GetFirst();
        } else {
            Deleter()(ptr_.GetFirst());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void* Release() {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;
        return old;
    };

    void Reset(void* ptr = nullptr) {
        auto old = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        if constexpr (std::is_same_v<Deleter, Slug>) {
            delete old;
        } else {
            Deleter()(old);
        }
    };

    void Swap(UniquePtr& other) {
        std::swap(ptr_.GetFirst(), other.ptr_.GetFirst());
        std::swap(ptr_.GetSecond(), other.ptr_.GetSecond());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    void* Get() const {
        return ptr_.GetFirst();
    };
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    };
    explicit operator bool() const {
        return ptr_.GetFirst() != nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    void* operator->() const {
        return ptr_.GetFirst();
    };

    // protected:
    CompressedPair<void*, Deleter> ptr_;
};