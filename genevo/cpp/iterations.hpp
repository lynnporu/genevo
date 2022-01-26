#pragma once

#include <iterator>
#include <cstddef>

template<typename T>
struct Iterator {

	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;

	Iterator(T* ptr) : m_ptr(ptr) {}

	T& operator*();
	T* operator->();
	Iterator& operator++();
	Iterator operator++(int);
	friend bool operator== (const Iterator& a, const Iterator& b);
    friend bool operator!= (const Iterator& a, const Iterator& b);

private:
    T* m_ptr;

};
