#ifndef _CUCKOO_H_
#define _CUCKOO_H_

#include <vector>
#include <stdlib.h>
#include <math.h>
#include "hasher.h"

#define EPSILON 7/5.

#define LOG_E(x) (std::log(x)/std::log(1+EPSILON))
#define MAX_LOOP	 		((int)ceil(3*LOG_E(this->T_1.size())))
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
	size_t size;

	_CuckooHashTable(size_t);

	double load_factor() const;
	size_t new_table_size() const;
	void rehash();
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
	size(0)
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
	this->size = h.size;

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
	return double(this->size) / this->T_1.size();
}

template<class T, class H>
size_t _CuckooHashTable<T, H>::new_table_size() const
{
	return 2 * this->T_1.size();
}

template<class T, class H>
void _CuckooHashTable<T, H>::rehash()
{
	size_t new_table_size = this->new_table_size();
	_CuckooHashTable new_table(new_table_size);

	typename std::vector<const T*>::iterator it;

	for(it = this->T_1.begin(); it != this->T_1.end(); it++ )
	{
		const T *key = *it;
		if( key )
			new_table.insert(*key);
	}
	for(it = this->T_2.begin(); it != this->T_2.end(); it++ )
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
        this->rehash();

    this->do_insert(key);
}

template<class T, class H>
void _CuckooHashTable<T, H>::do_insert(const T& key)
{
	size_t h1, h2;

	T *kicked_out, *value, *k = new T(key);

	bool inserted = false;

	while( !inserted )
	{
		value = k;

		// First check if the key is already stored.
		// If so, update its value and stop.
		if( this->set_if_present(value, key) )
			return;

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

			// T_1[h1] is not free. Kick out that key and continue.
			kicked_out = new T(*this->T_1[h1]);
			delete this->T_1[h1];
			this->T_1[h1] = value;

			h2 = this->h_2.hash(*kicked_out);
			// Now, if T_2[h2] is free, we place the kicked out key there.
			if( this->T_2[h2] == NULL )
			{
				this->T_2[h2] = kicked_out;
				inserted = true;
				break;
			}

			// Otherwise, we have to loop.
			value = kicked_out;
		}

		if( !inserted )
			this->rehash();
	}

	this->size++;
}

template<class T, class H>
void _CuckooHashTable<T, H>::remove(const T& key)
{
	this->set_if_present(NULL, key);
}

template<class T, class H>
bool _CuckooHashTable<T, H>::set_if_present(T *value, const T& key)
{
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
	const T *value = this->lookup(key);
	return value != NULL;
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
	return this->size == 0;
}

template<class T, class H>
std::vector<const T*> _CuckooHashTable<T, H>::items() const
{
	std::vector<const T*> items;
	// TODO

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

// Define type alias for cuckoo hash tables that use the standard hashing
// function. Tests, for example, will use other non-random hashers.
template<class T>
using CuckooHashTable = _CuckooHashTable<T, Hasher>;

#endif
