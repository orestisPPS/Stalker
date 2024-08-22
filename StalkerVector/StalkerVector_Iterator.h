//
// Created by hal9000 on 7/7/24.
//

#ifndef STALKER_CONTIGUOUSMEMORYITERATOR_H
#define STALKER_CONTIGUOUSMEMORYITERATOR_H

#include <iterator>

template <typename T>
class Iterator{
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    Iterator(pointer ptr) : _ptr(ptr) {}

    reference operator*() const { return *_ptr; }
    pointer operator->() { return _ptr; }

    // Prefix increment
    Iterator& operator++() { _ptr++; return *this; }

    // Postfix increment
    Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

    friend bool operator==(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; }
    friend bool operator!=(const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; }

private:
    pointer _ptr;
};

#endif //STALKER_CONTIGUOUSMEMORYITERATOR_H
