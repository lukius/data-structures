#ifndef __MMHEAP_H__
#define __MMHEAP_H__

#include <algorithm>
#include <assert.h>
#include <stddef.h>
#include <vector>

typedef bool LevelType;

#define _HEAP					(*(this->heap))
#define _PARENT_IDX(i)			((i-1)/2)
#define _GRANDPARENT_IDX(i)		(_PARENT_IDX(_PARENT_IDX(i)))
#define _LAST_IDX				(this->heap->size()-1)
// h[i] has children iff 2i + 1 < |h| => i = (|h|-2)/2 is the last non-leaf
// node.
#define _LAST_NON_LEAF_IDX		((this->heap->size()-2)/2)

#define _HAS_PARENT(i)			(i > 0)
#define _HAS_GRANDPARENT(i)		(i > 2)
#define _HAS_CHILDREN(i)		(2*i+1 < this->heap->size())
#define _HAS_GRANDCHILDREN(i)	(4*i+3 < this->heap->size())

// Check whether a new tree level starts at index i. For this, just check
// if (i+1) is a power of 2.
#define _NEW_LEVEL_AT(i)		(((i+1)&(i)) == 0)
#define _MAX_LEVEL				(false)
#define _MIN_LEVEL				(true)


template<class T>
class MinMaxHeap
{
	std::vector<T> *heap;

	LevelType last_level_type;

	void _sift_up(size_t, LevelType);
	void _sift_up_min(size_t);
	void _sift_up_max(size_t);

	void _sift_down(size_t, LevelType);
	void _sift_down_min(size_t);
	void _sift_down_max(size_t);

	size_t _max_idx() const;
	size_t _min_child_idx(size_t) const;
	size_t _max_child_idx(size_t) const;
	size_t _min_grandchild_idx(size_t) const;
	size_t _max_grandchild_idx(size_t) const;

public:
	MinMaxHeap();
	MinMaxHeap(const MinMaxHeap&);
	MinMaxHeap(const std::vector<T>&);
	~MinMaxHeap();

	void insert(const T&);
	const T &peek_min() const;
	const T &peek_max() const;

	T extract_min();
	T extract_max();

	bool is_empty() const;
	size_t size() const;

	const MinMaxHeap &operator=(const MinMaxHeap&);
};


template<class T>
MinMaxHeap<T>::MinMaxHeap() :
	heap(new std::vector<T>),
last_level_type(_MAX_LEVEL)
{
}

template<class T>
MinMaxHeap<T>::MinMaxHeap(const MinMaxHeap<T> &h) :
	heap(new std::vector<T>(*h.heap)),
last_level_type(_MAX_LEVEL)
{
}

template<class T>
MinMaxHeap<T>::MinMaxHeap(const std::vector<T> &v) :
	heap(new std::vector<T>(v))
{
	// In essence, Floyd's linear time algorithm to build binary heaps. The
	// difference is that sift_down takes the min-max heap properties into
	// account. For this, we also need to know whether the last level is
	// a max or min level. This is done first (in logarithmic time).

	size_t n = this->size();
	this->last_level_type = _MAX_LEVEL;
	while(n)
	{
		this->last_level_type = !this->last_level_type;
		n >>= 1;
	}

	LevelType curr_level_type = !this->last_level_type;
	for(ssize_t i = _LAST_NON_LEAF_IDX; i >= 0; i--)
	{
		this->_sift_down(i, curr_level_type);
		if(_NEW_LEVEL_AT(i))
			curr_level_type = !curr_level_type;
	}
}

template<class T>
MinMaxHeap<T>::~MinMaxHeap()
{
	delete this->heap;
}

template<class T>
const MinMaxHeap<T> &MinMaxHeap<T>::operator=(const MinMaxHeap<T> &h)
{
	if(this != &h)
	{
		delete this->heap;
		this->heap = new std::vector<T>(*h.heap);
		this->last_level_type = h.last_level_type;
	}

	return *this;
}

template<class T>
bool MinMaxHeap<T>::is_empty() const
{
	return this->size() == 0;
}

template<class T>
size_t MinMaxHeap<T>::size() const
{
	return this->heap->size();
}

template<class T>
size_t MinMaxHeap<T>::_max_idx() const
{
	assert(!this->is_empty());

	if(this->size() == 1)
		return 0;

	if(this->size() == 2)
		return 1;

	// The maximum element can either be the root of the left subtree or the
	// root of the right subtree.
	return (_HEAP[1] > _HEAP[2]) ? 1 : 2;
}

template<class T>
const T &MinMaxHeap<T>::peek_min() const
{
	assert(!this->is_empty());

	// The minimum element is just the root of the tree.
	return _HEAP[0];
}

template<class T>
const T &MinMaxHeap<T>::peek_max() const
{
	assert(!this->is_empty());

	return _HEAP[this->_max_idx()];
}

template<class T>
void MinMaxHeap<T>::insert(const T &value)
{
	// push_back has amortized O(1) running time, which guarantees amortized
	// O(log n) running time for insert (being n the number of nodes in the
	// heap).
	this->heap->push_back(value);
	if(_NEW_LEVEL_AT(_LAST_IDX))
		this->last_level_type = !this->last_level_type;
	this->_sift_up(_LAST_IDX, this->last_level_type);
}

template<class T>
void MinMaxHeap<T>::_sift_up(size_t i, LevelType level_type)
{
	size_t j;
	T heap_i = _HEAP[i];

	if(level_type == _MIN_LEVEL)
	{
		if(_HAS_PARENT(i) && heap_i > (_HEAP[j = _PARENT_IDX(i)]))
		{
			std::swap(_HEAP[i], _HEAP[j]);
			this->_sift_up_max(j);
		}
		else
			this->_sift_up_min(i);
	}
	else
	{
		if(_HAS_PARENT(i) && heap_i < (_HEAP[j = _PARENT_IDX(i)]))
		{
			std::swap(_HEAP[i], _HEAP[j]);
			this->_sift_up_min(j);
		}
		else
			this->_sift_up_max(i);
	}
}

template<class T>
void MinMaxHeap<T>::_sift_up_min(size_t i)
{
	size_t j;
	T heap_i = _HEAP[i];

	while(_HAS_GRANDPARENT(i) && heap_i < (_HEAP[j = _GRANDPARENT_IDX(i)]))
	{
		std::swap(_HEAP[i], _HEAP[j]);
		i = j;
	}
}

template<class T>
void MinMaxHeap<T>::_sift_up_max(size_t i)
{
	size_t j;
	T heap_i = _HEAP[i];

	while(_HAS_GRANDPARENT(i) && heap_i > (_HEAP[j = _GRANDPARENT_IDX(i)]))
	{
		std::swap(_HEAP[i], _HEAP[j]);
		i = j;
	}
}

template<class T>
T MinMaxHeap<T>::extract_min()
{
	assert(!this->is_empty());

	T min = this->peek_min();
	std::swap(_HEAP[0], _HEAP[_LAST_IDX]);
	this->heap->erase(this->heap->begin() + _LAST_IDX);
	if(_NEW_LEVEL_AT(_LAST_IDX+1))
		this->last_level_type = !this->last_level_type;
	this->_sift_down_min(0);

	return min;
}

template<class T>
T MinMaxHeap<T>::extract_max()
{
	assert(!this->is_empty());

	size_t max_idx = this->_max_idx();
	T max = _HEAP[max_idx];
	std::swap(_HEAP[max_idx], _HEAP[_LAST_IDX]);
	this->heap->erase(this->heap->begin() + _LAST_IDX);
	if(_NEW_LEVEL_AT(_LAST_IDX+1))
		this->last_level_type = !this->last_level_type;
	this->_sift_down_max(max_idx);

	return max;
}

template<class T>
void MinMaxHeap<T>::_sift_down(size_t i, LevelType level_type)
{
	if(level_type == _MIN_LEVEL)
		this->_sift_down_min(i);
	else
		this->_sift_down_max(i);
}

template<class T>
void MinMaxHeap<T>::_sift_down_min(size_t i)
{
	size_t j;
	T heap_i = _HEAP[i];

	while(	_HAS_GRANDCHILDREN(i) &&
			heap_i > (_HEAP[j = this->_min_grandchild_idx(i)]))
	{
		std::swap(_HEAP[i], _HEAP[j]);
		i = j;

		if(heap_i > _HEAP[j =_PARENT_IDX(i)])
			std::swap(_HEAP[i], _HEAP[j]);
	}

	if(	_HAS_CHILDREN(i) &&
		heap_i > (_HEAP[j = this->_min_child_idx(i)]))
		std::swap(_HEAP[i], _HEAP[j]);
}

template<class T>
void MinMaxHeap<T>::_sift_down_max(size_t i)
{
	size_t j;
	T heap_i = _HEAP[i];

	while(	_HAS_GRANDCHILDREN(i) &&
			heap_i < (_HEAP[j = this->_max_grandchild_idx(i)]))
	{
		std::swap(_HEAP[i], _HEAP[j]);
		i = j;

		if(heap_i < _HEAP[j =_PARENT_IDX(i)])
			std::swap(_HEAP[i], _HEAP[j]);
	}

	if(	_HAS_CHILDREN(i) &&
		heap_i < (_HEAP[j = this->_max_child_idx(i)]))
		std::swap(_HEAP[i], _HEAP[j]);
}

template<class T>
size_t MinMaxHeap<T>::_min_child_idx(size_t i) const
{
	assert(_HAS_CHILDREN(i));

	size_t j, min, n = this->heap->size();
	j = min = 2*i + 1;

	if(++j < n && _HEAP[min] > _HEAP[j])
		min = j;

	return min;
}

template<class T>
size_t MinMaxHeap<T>::_max_child_idx(size_t i) const
{
	assert(_HAS_CHILDREN(i));

	size_t j, max, n = this->heap->size();
	j = max = 2*i + 1;

	if(++j < n && _HEAP[max] < _HEAP[j])
		max = j;

	return max;
}

template<class T>
size_t MinMaxHeap<T>::_min_grandchild_idx(size_t i) const
{
	assert(_HAS_GRANDCHILDREN(i));

	size_t j, min, n = this->heap->size();
	j = min = 4*i + 3;

	if(++j < n && _HEAP[min] > _HEAP[j])
		min = j;
	if(++j < n && _HEAP[min] > _HEAP[j])
		min = j;
	if(++j < n && _HEAP[min] > _HEAP[j])
		min = j;

	return min;
}

template<class T>
size_t MinMaxHeap<T>::_max_grandchild_idx(size_t i) const
{
	assert(_HAS_GRANDCHILDREN(i));

	size_t j, max, n = this->heap->size();
	j = max = 4*i + 3;

	if(++j < n && _HEAP[max] < _HEAP[j])
		max = j;
	if(++j < n && _HEAP[max] < _HEAP[j])
		max = j;
	if(++j < n && _HEAP[max] < _HEAP[j])
		max = j;

	return max;
}

#endif
