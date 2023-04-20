#pragma once

#include <cstddef> // for std::nullptr_t
#include <utility> // for std::exchange / std::swap


class SimpleCounter {
public:
    size_t IncRef();
    size_t DecRef();
    size_t RefCount() const;

private:
    size_t count_ = 0;
};
a#pragma once

#include <cstddef>  // for std::nullptr_t
#include <utility>  // for std::exchange / std::swap

class SimpleCounter {
public:
    // SimpleCounter(size_t val) : ref_count_(val){};
    size_t IncRef() {
        ++ref_count_;
        return ref_count_;
    };

    size_t DecRef() {
        if (ref_count_ == 0) {
            return 0;
        }
        --ref_count_;
        return ref_count_;
    };

    size_t RefCount() const {
        return ref_count_;
    };

    size_t ref_count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    // Increase reference strong_counter.
    void IncRef() {
        counter_.IncRef();
    };

    // Decrease reference strong_counter.
    // Destroy object using Deleter when the last instance dies.
    void DecRef() {
        if (counter_.DecRef() == 0) {
            Deleter::Destroy(static_cast<Derived*>(this));
        }
    };

    // Get current strong_counter value (the number of strong references).
    size_t RefCount() const {
        return counter_.RefCount();
    };

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;

public:
    // Constructors
    IntrusivePtr(){};
    IntrusivePtr(std::nullptr_t){};
    IntrusivePtr(T* ptr) {
        object = ptr;
        if (object != nullptr) {
            object->IncRef();
        }
    };

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) {
        object = other.object;
        if (object != nullptr) {
            object->IncRef();
        }
    };

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) {
        object = other.object;
        other.object = nullptr;
    };

    IntrusivePtr(const IntrusivePtr& other) {
        object = other.object;
        if (object != nullptr) {
            object->IncRef();
        }
    };

    IntrusivePtr(IntrusivePtr&& other) {
        object = other.object;
        other.object = nullptr;
    };

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (object == other.object) {
            return *this;
        }
        Reset();
        object = other.object;
        if (object != nullptr) {
            object->IncRef();
        }
        return *this;
    };

    template <class U>
    IntrusivePtr& operator=(const IntrusivePtr<U>& other) {
        if (object == other.object) {
            return *this;
        }
        Reset();
        object = other.object;
        if (object != nullptr) {
            object->IncRef();
        }
        return *this;
    };

    IntrusivePtr& operator=(IntrusivePtr&& other) {
        if (object == other.object) {
            return *this;
        }
        Reset();
        object = other.object;
        other.object = nullptr;
        return *this;
    };

    template <class U>
    IntrusivePtr& operator=(IntrusivePtr&& other) {
        if (object == other.object) {
            return *this;
        }
        Reset();
        object = other.object;
        other.object = nullptr;
        return *this;
    };

    // Destructor
    ~IntrusivePtr() {
        if (object == nullptr) {
        } else {
            object->DecRef();
        }
    };

    // Modifiers
    void Reset() {
        if (object == nullptr) {
        } else {
            object->DecRef();
        }
        object = nullptr;
    };

    void Reset(T* ptr) {
        if (object == nullptr) {
        } else {
            object->DecRef();
        }
        object = ptr;
        object->IncRef();
    };

    template <class U>
    void Reset(U* ptr) {
        if (object == nullptr) {
        } else {
            object->DecRef();
        }
        object = ptr;
        object->IncRef();
    };

    void Swap(IntrusivePtr& other) {
        std::swap(object, other.object);
    };

    template <typename U>
    void Swap(IntrusivePtr<U>& other) {
        std::swap(object, other.object);
    };

    // Observers
    T* Get() const {
        return object;
    };

    T& operator*() const {
        return *object;
    };

    T* operator->() const {
        return object;
    };

    size_t UseCount() const {
        if (object == nullptr) {
            return 0;
        }
        return object->RefCount();
    };
    explicit operator bool() const {
        return object != nullptr && object->RefCount() != 0;
    };

    T* object = nullptr;
};

/*template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    IntrusivePtr<T> ans;
    ans.object = new T(std::forward<Args>(args)...);
    ans.object->IncRef();
    return ans;
};*/

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
}
struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    // Increase reference counter.
    void IncRef();

    // Decrease reference counter.
    // Destroy object using Deleter when the last instance dies.
    void DecRef();

    // Get current counter value (the number of strong references).
    size_t RefCount() const;

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;

public:
    // Constructors
    IntrusivePtr();
    IntrusivePtr(std::nullptr_t);
    IntrusivePtr(T* ptr);

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other);

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other);

    IntrusivePtr(const IntrusivePtr& other);
    IntrusivePtr(IntrusivePtr&& other);

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other);
    IntrusivePtr& operator=(IntrusivePtr&& other);

    // Destructor
    ~IntrusivePtr();

    // Modifiers
    void Reset();
    void Reset(T* ptr);
    void Swap(IntrusivePtr& other);

    // Observers
    T* Get() const;
    T& operator*() const;
    T* operator->() const;
    size_t UseCount() const;
    explicit operator bool() const;
};

template <typename T, typename ...Args>
IntrusivePtr<T> MakeIntrusive(Args&& ...args);
