// This file is part of etceteras library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


namespace etceteras {
    
    
    template<class E>
    class unexpected {
    private:
        E value_;
        
    public:
    
        unexpected() = delete;
        explicit constexpr unexpected(E const& e): value_(e) { }
        explicit constexpr unexpected(E&& e): value_(std::move(e)) { }
        constexpr E const& value() const& noexcept { return value_; }
        constexpr E& value() & noexcept { return value_; }
        constexpr E&& value() && noexcept { return std::move(value_); }
        
    }; // unexpected


    template<class E> constexpr unexpected<std::decay_t<E>> make_unexpected(E&& e) {
        return unexpected<typename std::decay<E>::type>{std::forward<E>(e)};
    }
    
    
    template<typename T, typename E>
    class expected {
    private:
    
        bool has_value_;
        union data {
            T value;
            E error;
            explicit data(T const& v): value{v} { }
            explicit data(T&& v): value(std::move(v)) { }
            explicit data(unexpected<E> const& e): error{e.value()} { }
            explicit data(unexpected<E>&& e): error{std::move(e).value()} { }
        } data_;
        
    public:
        
        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;
        
        
        expected() = delete;
        
        
        constexpr expected(expected const& other)
            : has_value_{other.has_value_} {
            if(has_value_)
                new(&data_.value) T(other.data_.value);
            else
                new(&data_.error) T(other.data_.error);
        }
        
        
        constexpr expected& operator = (expected const& other) {
            if(has_value_ == other.has_value_) {
                if(has_value_)
                    data_.value = other.data_.value;
                else
                    data_.error = other.data_.error;
            } else {
                if(has_value_) {
                    data_.value.~T();
                    new(&data_.error) E(other.data_.error);
                } else {
                    data_.error.~E();
                    new(&data_.value) E(other.data_.value);
                }
                has_value_ = other.has_value_;
            }
            return *this;
        }
        
        
        constexpr expected(expected&& other)
            : has_value_{other.has_value_} {
            if(has_value_)
                new(&data_.value) T(std::move(other.data_.value));
            else
                new(&data_.error) E(std::move(other.data_.error));
        }
        
        
        constexpr expected& operator = (expected&& other) {
            if(has_value_ == other.has_value_) {
                if(has_value_)
                    data_.value = std::move(other.data_.value);
                else
                    data_.error = std::move(other.data_.error);
            } else {
                if(has_value_) {
                    data_.value.~T();
                    new(&data_.error) E(std::move(other.data_.error));
                } else {
                    data_.error.~E();
                    new(&data_.value) E(std::move(other.data_.value));
                }
                has_value_ = other.has_value_;
            }
            return *this;
        }
        
        
        ~expected() {
            if(has_value_)
                data_.value.~T();
            else
                data_.error.~E();
        }
        
        
        explicit constexpr expected(T const& value): has_value_{true}, data_{value} { }
        explicit constexpr expected(T&& value): has_value_{true}, data_{std::move(value)} { }
        constexpr expected(unexpected<E> const& error): has_value_{false}, data_{error} { }
        constexpr expected(unexpected<E>&& error): has_value_{false}, data_{std::move(error)} { }
        constexpr T* operator -> () noexcept { return &data_.value; }
        constexpr T const* operator -> () const noexcept { return &data_.value_; }
        constexpr T& operator * () & noexcept { return data_.value; }
        constexpr T const& operator * () const & noexcept { return data_.value; }
        constexpr T&& operator * () && noexcept { return std::move(data_.value); }
        constexpr T& value() & noexcept { return data_.value; }
        constexpr T const& value() const & noexcept { return data_.value; }
        constexpr T&& value() && noexcept { return std::move(data_.value); }
        constexpr E& error() & noexcept { return data_.error; }
        constexpr E const& error() const& noexcept { return data_.error; }
        constexpr E&& error() && noexcept { return std::move(data_.error); }
        constexpr bool has_value() const noexcept { return has_value_; }
        explicit constexpr operator bool () const noexcept { return has_value_; }
        
        constexpr T const& value_or(T const& value) const & {
            return has_value_ ? data_.value : value;
        }
        
        
        constexpr T&& value_or(T&& value) && {
            return has_value_ ? std::move(data_.value) : std::move(value);
        }
    }; // expected
    
    
    template<typename E>
    class expected<void, E> {
    private:
    
        bool has_value_;
        E error_;
        
    public:
    
        using value_type = void;
        using error_type = E;
        using unexpected_type = unexpected<E>;
        
        expected(): has_value_{true} { }
        
        
        constexpr expected(expected const& other)
            : has_value_{other.has_value_} {
            if(!has_value_)
                new(&error_) E(other.error_);
        }
        
        
        constexpr expected& operator = (expected const& other) {
            if(has_value_ == other.has_value_) {
                if(!has_value_)
                    error_ = other.error_;
            } else {
                if(has_value_) {
                    new(&error_) E(other.error_);
                } else {
                    error_.~E();
                }
                has_value_ = other.has_value_;
            }
            return *this;
        }
        
        
        constexpr expected(expected&& other)
            : has_value_{other.has_value_} {
            if(!has_value_)
                new(&error_) E(std::move(other.error_));
        }
        
        
        constexpr expected& operator = (expected&& other) {
            if(has_value_ == other.has_value_) {
                if(!has_value_)
                    error_ = std::move(other.error_);
            } else {
                if(has_value_) {
                    new(&error_) E(std::move(other.error_));
                } else {
                    error_.~E();
                }
                has_value_ = other.has_value_;
            }
            return *this;
        }
        
        
        ~expected() {
            if(!has_value_)
                error_.~E();
        }
        
        
        constexpr expected(unexpected<E> const& error): has_value_{false}, error_{error.value()} { }
        constexpr expected(unexpected<E>&& error): has_value_{false}, error_{std::move(error).value()} { }
        constexpr E& error() & noexcept { return error_; }
        constexpr E const& error() const& noexcept { return error_; }
        constexpr E&& error() && noexcept { return std::move(error_); }
        constexpr bool has_value() const noexcept { return has_value_; }
        explicit constexpr operator bool () const noexcept { return has_value_; }
        
    }; // expected


} // namespace etceteras
