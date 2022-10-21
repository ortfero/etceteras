// This file is part of etceteras library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <cstddef>
#include <new>


namespace etceteras {
    
    
    namespace detail {
        
        template<typename T>
        struct pyramid_node {
            union {
                char buffer[sizeof(T)];
                T item;
            } data;
            pyramid_node* previous_node;
            pyramid_node* next_node;
        }; // pyramid_node
       
        
        using pyramid_size_type = std::size_t;
        
        
        template<typename T>
        struct pyramid_page {
            pyramid_page* next_page;
            pyramid_size_type capacity;
            pyramid_node<T> nodes[1];
        }; // pyramid_page
        
        
    } // namespace detail
    
    
    template<typename T, detail::pyramid_size_type F = 16>
    class pyramid {
        
        detail::pyramid_size_type capacity_;
        detail::pyramid_size_type size_;
        detail::pyramid_page<T> pages_;
        detail::pyramid_size_type next_page_estimate_;
        detail::pyramid_node<T> free_nodes_;
        detail::pyramid_node<T> occupied_nodes_;
        
    public:
    
        using size_type = detail::pyramid_size_type;
        using value_type = T;
        
        static size_type constexpr factor = F;
        
        
        class iterator {
        friend class pyramid;
        friend class const_iterator;
            detail::pyramid_node<T>* node_;
           
            iterator(detail::pyramid_node<T>* node): node_{node} { }
            
        public:
            
            iterator(iterator const&) = default;
            iterator& operator = (iterator const&) = default;
            
            bool operator == (iterator const& other) const noexcept {
                return node_ == other.node_;
            }
            
            bool operator != (iterator const& other) const noexcept {
                return node_ != other.node_;
            }
            
            T& operator * () const noexcept {
                return node_->data.item;
            }
            
            T* operator -> () const noexcept {
                return &node_->data.item;
            }
            
            iterator& operator ++ () noexcept {
                node_ = node_->next_node;
                return *this;
            }
                        
            iterator operator ++ (int) noexcept {
                auto const last = *this;
                node_ = node_->next_node;
                return last;
            }
            
            iterator& operator -- () noexcept {
                node_ = node_->previous_node;
                return *this;
            }

            iterator operator -- (int) noexcept {
                auto const last = *this;
                node_ = node_->previous_node;
                return last;
            }

        }; // const_iterator
        
        
        class const_iterator {
        friend class pyramid;
            detail::pyramid_node<T>* node_;
            
            const_iterator(detail::pyramid_node<T>* node): node_{node} { }
            
        public:
            
            const_iterator(const_iterator const&) = default;
            const_iterator& operator = (const_iterator const&) = default;
            const_iterator(iterator const& it) noexcept: node_{it.node_} { }
            
            bool operator == (const_iterator const& other) const noexcept {
                return node_ == other.node_;
            }
            
            bool operator != (const_iterator const& other) const noexcept {
                return node_ != other.node_;
            }
            
            T const& operator * () const noexcept {
                return node_->data.item;
            }
            
            T const* operator -> () const noexcept {
                return &node_->data.item;
            }
            
            const_iterator& operator ++ () noexcept {
                node_ = node_->next_node;
                return *this;
            }
            
            const_iterator operator ++ (int) noexcept {
                auto const last = *this;
                node_ = node_->next_node;
                return last;
            }
            
            iterator& operator -- () noexcept {
                node_ = node_->previous_node;
                return *this;
            }

            iterator operator -- (int) noexcept {
                auto const last = *this;
                node_ = node_->previous_node;
                return last;
            }
        }; // const_iterator
        
        
        pyramid() noexcept {
            init();
        }
        
        
        ~pyramid() {
            clear();
        }
        
        
        pyramid(pyramid const& other) {
            init();
            for(auto const& item: other)
                insert(item);
        }
        
        
        pyramid& operator = (pyramid const& other) {
            clear();
            for(auto const& item: other)
                insert(item);
        }
        
        
        size_type size() const noexcept {
            return size_;
        }
        
        
        size_type capacity() const noexcept {
            return capacity_;
        }
        
        
        bool empty() const noexcept {
            return size_ == 0;
        }
        
        
        void clear() noexcept {
            for(auto* node = occupied_nodes_.next_node;
                node != &occupied_nodes_;
                node = node->next_node)
                    node->data.item.~T();
            auto* page = pages_.next_page;
            while(page != &pages_) {
                auto* disposable = page;
                page = page->next_page;
                std::free(disposable);
            }
            init();
        }

        
        const_iterator begin() const noexcept {
            return const_iterator{occupied_nodes_.next_node};
        }
        
        
        const_iterator end() const noexcept {
            return const_iterator{&occupied_nodes_};
        }


        iterator begin() {
            return iterator{occupied_nodes_.next_node};
        }
        
        
        iterator end() {
            return iterator{&occupied_nodes_};
        }

        
        
        iterator insert(T const& item) {
            auto* node = allocate_node();
            new(node->data.buffer) T(item);
            return iterator{node};
        }
        
        
        iterator insert(T&& item) {
            auto* node = allocate_node();
            new(node->data.buffer) T(std::move(item));
            return iterator{node};
        }
        
        
        const_iterator erase(const_iterator it) {
            return const_iterator{free_node(it.node_)};
        }
        
        
        iterator erase(iterator it) {
            return iterator{free_node(it.node_)};
        }

        
    private:
    
        void init() {
            capacity_ = 0;
            size_ = 0;
            pages_.next_page = &pages_;
            pages_.capacity = 0;
            next_page_estimate_ = F;
            free_nodes_.previous_node = &free_nodes_;
            free_nodes_.next_node = &free_nodes_;
            occupied_nodes_.previous_node = &occupied_nodes_;
            occupied_nodes_.next_node = &occupied_nodes_;
        }

    
        detail::pyramid_node<T>* allocate_node() {
            if(capacity_ == size_) {
                auto const page_capacity = next_page_estimate_ - size_;
                auto const next_page_estimate = next_page_estimate_ * F;
                if(next_page_estimate < next_page_estimate_)
                    throw std::bad_alloc{};
                next_page_estimate_ = next_page_estimate;
                auto const page_size = sizeof(detail::pyramid_page<T>);
                auto const nodes_size = (page_capacity - 1) * sizeof(detail::pyramid_node<T>);
                auto* new_page = static_cast<detail::pyramid_page<T>*>(std::malloc(page_size + nodes_size));
                if(!new_page)
                    throw std::bad_alloc{};
                new_page->next_page = pages_.next_page;
                new_page->capacity = page_capacity;
                pages_.next_page = new_page;
                for(auto* node = new_page->nodes; node != new_page->nodes + page_capacity; ++node) {
                    node->previous_node = node - 1;
                    node->next_node = node + 1;
                }
                new_page->nodes[0].previous_node = &free_nodes_;
                free_nodes_.next_node = new_page->nodes;
                new_page->nodes[page_capacity - 1].next_node = &free_nodes_;
                free_nodes_.previous_node = new_page->nodes + page_capacity - 1;
                capacity_ += page_capacity;
            }
            auto* allocated_node = free_nodes_.next_node;
            allocated_node->next_node->previous_node = allocated_node->previous_node;
            allocated_node->previous_node->next_node = allocated_node->next_node;
            allocated_node->next_node = &occupied_nodes_;
            allocated_node->previous_node = occupied_nodes_.previous_node;
            occupied_nodes_.previous_node->next_node = allocated_node;
            occupied_nodes_.previous_node = allocated_node;
            ++size_;
            return allocated_node;
        }
        
        
        detail::pyramid_node<T>* free_node(detail::pyramid_node<T>* node) {
            auto* next_node = node->next_node;
            node->data.item.~T();
            node->previous_node->next_node = node->next_node;
            node->next_node->previous_node = node->previous_node;
            node->previous_node = &free_nodes_;
            node->next_node = free_nodes_.next_node;
            free_nodes_.next_node->previous_node = node;
            free_nodes_.next_node = node;
            --size_;
            return next_node;
        }
        
        
    }; // pyramid
    
    
} // namespace etceteras
