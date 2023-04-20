#pragma once

#include <type_traits>
#include <utility>

template <typename F, typename S, bool = std::is_empty_v<F> && !std::is_final_v<F>,
          bool = std::is_empty_v<S> && !std::is_final_v<S>>
class CompressedPair;

template <typename F, typename S>
class CompressedPair<F, S, false, false> {

public:
    CompressedPair(){};

    template <class U, class V>
    CompressedPair(U&& first, V&& second)
        : first_(std::forward<U>(first)), second_(std::forward<V>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_{};
    S second_{};
};

template <typename F, typename S>
class CompressedPair<F, S, true, false> : F {

public:
    CompressedPair(){};

    template <class U, class V>
    CompressedPair(U&& first, V&& second)
        : F(std::forward<U>(first)), second_(std::forward<V>(second)) {
    }

    F& GetFirst() {
        return (*this);
    }

    const F& GetFirst() const {
        return (*this);
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_{};
};

template <typename F, typename S>
class CompressedPair<F, S, false, true> : S {

public:
    CompressedPair(){};

    template <class U, class V>
    CompressedPair(U&& first, V&& second)
        : first_(std::forward<U>(first)), S(std::forward<V>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return (*this);
    };

    const S& GetSecond() const {
        return (*this);
    };

private:
    F first_{};
};

template <typename F, typename S>
class CompressedPair<F, S, true, true> : F, S {

public:
    CompressedPair(){};

    template <class U, class V>
    CompressedPair(U&& first, V&& second) : F(std::forward<U>(first)), S(std::forward<V>(second)) {
    }

    F& GetFirst() {
        return (*this);
    }

    const F& GetFirst() const {
        return (*this);
    }

    S& GetSecond() {
        return (*this);
    };

    const S& GetSecond() const {
        return (*this);
    };
};

template <typename F>
class CompressedPair<F, F, true, true> : F {

public:
    CompressedPair(){};

    // CompressedPair(const F& first, F&& second) : F(first) {};

    template <typename U, typename V>
    CompressedPair(U&& first, V&& second)
        : F(std::forward<U>(first), first(std::forward<V>(second))){};

    // CompressedPair(F&& first, const F& second)  : F(std::move(first)) {
    // }

    F& GetFirst() {
        return (*this);
    }

    const F& GetFirst() const {
        return (*this);
    }

    F& GetSecond() {
        return (*this);
    };

    const F& GetSecond() const {
        return (*this);
    };

private:
    F first_{};
};

template <typename F>
class CompressedPair<F, F, false, false> {

public:
    CompressedPair(){};

    template <class U>
    CompressedPair(U&& first, U&& second)
        : first_(std::forward<U>(first)), second_(std::forward<U>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    F& GetSecond() {
        return second_;
    };

    const F& GetSecond() const {
        return second_;
    };

private:
    F first_{};
    F second_{};
};
