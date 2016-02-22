#ifndef _CUCKOO_H_
#define _CUCKOO_H_

#include <vector>
#include <stdlib.h>
#include <math.h>
#include "hasher.h"

#define EPSILON 7/5.

#define MAX_LOOP	 		((int)ceil(3*LOG(this->T_1.size(), EPSILON)))
#define MAX_LOAD_FACTOR		(1./(1 + EPSILON))

#define INITIAL_SIZE 2


template<class T, class H>
class _CuckooHashTable
{
	// Cuckoo hashing implementation as presented in R. Pagh's and
	// F.F. Rodler's paper "Cuckoo Hashing".

private:
	std::vector<const T*> T_1, T_2;
	H h_1, h_2;
	size_t _size;

	_CuckooHashTable(size_t);

	double load_factor() const;
	size_t new_table_size() const;
	void rehash(bool);
	void do_insert(const T&);
	void copy_from(const _CuckooHashTable&);
	bool set_if_present(T *, const T&);
	void erase();

public:
	_CuckooHashTable();
	_CuckooHashTable(const _CuckooHashTable&);
	~_CuckooHashTable();

	void insert(const T&);
	void remove(const T&);

	bool is_empty() const;

	size_t size() const;

	bool contains(const T&) const;
	const T *lookup(const T&) const;

	std::vector<const T*> items() const;

	const _CuckooHashTable &operator=(const _CuckooHashTable&);
};


template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable() :
	_CuckooHashTable(INITIAL_SIZE)
{
}

template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable(size_t size) :
	T_1(size),
	T_2(size),
	h_1(H(size)),
	h_2(H(size)),
	_size(0)
{
	for(size_t i = 0; i < this->T_1.size(); ++i)
	{
		this->T_1[i] = NULL;
		this->T_2[i] = NULL;
	}
}

template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable(const _CuckooHashTable &h) :
	T_1(h.T_1.size()),
	T_2(h.T_2.size())
{
	this->copy_from(h);
}

template<class T, class H>
_CuckooHashTable<T, H>::~_CuckooHashTable()
{
	this->erase();
}

template<class T, class H>
void _CuckooHashTable<T, H>::copy_from(const _CuckooHashTable &h)
{
	this->h_1 = h.h_1;
	this->h_2 = h.h_2;
	this->_size = h._size;

	for(size_t i = 0; i < this->T_1.size(); ++i)
	{
		this->T_1[i] = (h.T_1[i] == NULL) ? NULL : new T(*h.T_1[i]);
		this->T_2[i] = (h.T_2[i] == NULL) ? NULL : new T(*h.T_2[i]);
	}
}

template<class T, class H>
void _CuckooHashTable<T, H>::erase()
{
	for(size_t i = 0; i < this->T_1.size(); ++i)
	{
		delete this->T_1[i];
		delete this->T_2[i];
	}
}

template<class T, class H>
double _CuckooHashTable<T, H>::load_factor() const
{
	return double(this->_size) / (2*this->T_1.size());
}

template<class T, class H>
size_t _CuckooHashTable<T, H>::new_table_size() const
{
	return 2 * this->T_1.size();
}

template<class T, class H>
void _CuckooHashTable<T, H>::rehash(bool should_resize)
{
	size_t new_table_size = should_resize ?
				this->new_table_size() :
				this->_size;

	_CuckooHashTable new_table(new_table_size);

	typename std::vector<const T*>::iterator it;

	for( it = this->T_1.begin(); it != this->T_1.end(); it++ )
	{
		const T *key = *it;
		if( key )
			new_table.insert(*key);
	}
	for( it = this->T_2.begin(); it != this->T_2.end(); it++ )
	{
		const T *key = *it;
		if( key )
			new_table.insert(*key);
	}

	*this = new_table;
}

template<class T, class H>
void _CuckooHashTable<T, H>::insert(const T& key)
{
	if( this->load_factor() >= MAX_LOAD_FACTOR )
		this->rehash(true);

	this->do_insert(key);
}

template<class T, class H>
void _CuckooHashTable<T, H>::do_insert(const T& key)
{
	T *value = new T(key);

	// First check if the key is already stored. If so, update its value
	// and stop.
	if( this->set_if_present(value, *value) )
		return;

	T *evicted;
	size_t h1, h2;
	bool inserted = false;

	while( !inserted )
	{
		for(int i = 0; i < MAX_LOOP; ++i)
		{
			h1 = this->h_1.hash(*value);
			// Easy case: T_1[h1] is free. Insert the key and stop.
			if( this->T_1[h1] == NULL )
			{
				this->T_1[h1] = value;
				inserted = true;
				break;
			}

			// T_1[h1] is not free. Kick out that key in order to make room
			// and continue.
			evicted = new T(*this->T_1[h1]);
			delete this->T_1[h1];
			this->T_1[h1] = value;

			// Now we need to put the evicted key back into the table.
			h2 = this->h_2.hash(*evicted);
			// Use T_2[h2] if it is free.
			if( this->T_2[h2] == NULL )
			{
				this->T_2[h2] = evicted;
				inserted = true;
				break;
			}

			// Otherwise, we have to kick out the key on T_2[h2] and
			// start over again.
			value = new T(*this->T_2[h2]);
			delete this->T_2[h2];
			this->T_2[h2] = evicted;
		}

		// MAX_LOOP exceeded. Renew hash functions and rehash the table.
		if( !inserted )
		{
			this->h_1.update();
			this->h_2.update();
			// No need to resize as the load factor did not increase.
			this->rehash(false);
		}
	}

	this->_size++;
}

template<class T, class H>
void _CuckooHashTable<T, H>::remove(const T& key)
{
	if( this->set_if_present(NULL, key) )
		this->_size--;
}

template<class T, class H>
bool _CuckooHashTable<T, H>::set_if_present(T *value, const T& key)
{
	// Update values of already placed keys. Can be useful for
	// types having satellite data (such as dictionary key-value pairs).

	size_t h1 = this->h_1.hash(key);
	if( this->T_1[h1] &&  *this->T_1[h1] == key )
	{
		delete this->T_1[h1];
		this->T_1[h1] = value;
		return true;
	}

	size_t h2 = this->h_2.hash(key);
	if( this->T_2[h2] &&  *this->T_2[h2] == key )
	{
		delete this->T_2[h2];
		this->T_2[h2] = value;
		return true;
	}

	return false;
}

template<class T, class H>
bool _CuckooHashTable<T, H>::contains(const T& key) const
{
	return this->lookup(key) != NULL;
}

template<class T, class H>
const T *_CuckooHashTable<T, H>::lookup(const T& key) const
{
	size_t h1 = this->h_1.hash(key);
	if( this->T_1[h1] &&  *this->T_1[h1] == key )
		return this->T_1[h1];

	size_t h2 = this->h_2.hash(key);
	if( this->T_2[h2] &&  *this->T_2[h2] == key )
		return this->T_2[h2];

	return NULL;
}

template<class T, class H>
bool _CuckooHashTable<T, H>::is_empty() const
{
	return this->_size == 0;
}

template<class T, class H>
size_t _CuckooHashTable<T, H>::size() const
{
	return this->_size;
}

template<class T, class H>
std::vector<const T*> _CuckooHashTable<T, H>::items() const
{
	// Returns an array holding the keys stored in the table
	// without any particular order.

	std::vector<const T*> items;

	typename std::vector<const T*>::const_iterator it;

	for(it = this->T_1.begin(); it != this->T_1.end(); it++ )
	{
		const T *key = *it;
		if( key )
			items.push_back(key);
	}
	for(it = this->T_2.begin(); it != this->T_2.end(); it++ )
	{
		const T *key = *it;
		if( key )
			items.push_back(key);
	}

	return items;
}

template<class T, class H>
const _CuckooHashTable<T, H> &_CuckooHashTable<T, H>::operator=(const _CuckooHashTable& h)
{
	if( this != &h )
	{
		this->erase();
		this->T_1.resize(h.T_1.size());
		this->T_2.resize(h.T_2.size());
		this->copy_from(h);
	}

	return *this;
}

// Type alias for cuckoo hash tables that use the standard hashing function.
template<class T>
using CuckooHashTable = _CuckooHashTable<T, Hasher>;

#endif
