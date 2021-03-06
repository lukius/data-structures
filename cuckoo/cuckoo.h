// Cuckoo hashing implementation as presented in R. Pagh's and F.F. Rodler's
// paper "Cuckoo Hashing".

// Each table stores tuples of the form <k, i>, where k is a key and i is such
// that L[i] = k, being L the list of inserted keys. The type alias used for
// these tuples is TableNode.
// In turn, the list L holds tuples <k, j, h> where j = 1, 2 is a table index,
// h is an index within T_j and T_j[h] = k. The type alias for these tuples is
// ListNode.
// This is to achieve expected O(n) running time on insertion, as this
// list can be used during a rehash to traverse the keys only (as opposed to
// searching for them throughout the whole table, which takes Theta(r) time).

// TODO:
//   * "If the hash tables have size r, we enforce that no more than r^2
//      insertions are performed without changing the hash functions. More
//      specifically, if r^2 insertions have been performed since the beginning
//      of the last rehash, we force a new rehash."


#ifndef _CUCKOO_H_
#define _CUCKOO_H_

#include <list>
#include <vector>
#include <tuple>
#include <algorithm>
#include <math.h>
#include "hasher.h"

#define EPSILON 7/5.

#define MAX_LOOP	 	((int)ceil(3*LOG(this->T_1->size(), EPSILON)))
#define MAX_LOAD_FACTOR		(1./(1 + EPSILON))

#define INITIAL_SIZE 2

// Type aliases

template<class T>
using ListNode = std::tuple<const T*, size_t, size_t>;

template<class T>
using NodeList = std::list<ListNode<T>>;

template<class T>
using NodeListIter = typename std::list<ListNode<T>>::iterator;

template<class T>
using NodeListConstIter = typename std::list<ListNode<T>>::const_iterator;

template<class T>
using TableNode = std::tuple<const T*, NodeListIter<T>>;

template<class T>
using Table = std::vector<TableNode<T>*>;


template<class T, class H>
class _CuckooHashTable
{
private:
	// The tables and their respective hashing functions.
	Table<T> *T_1, *T_2;
	H h_1, h_2;

	// List of the keys stored in the table following their insertion order.
	NodeList<T> *L;

	// Private methods

	_CuckooHashTable(size_t);

	void copy_from(const _CuckooHashTable&);
	void erase();
	bool set_if_present(const T *, const T&);

	double load_factor() const;
	size_t new_table_size() const;
	void rehash(bool);

	void do_insert(const T&);
	const T *insert_in(const T *, size_t);
	const T *evict(size_t, size_t);

	const TableNode<T> *lookup_node(const T&) const;
	TableNode<T> *new_node(const T*, size_t, size_t);

public:
	_CuckooHashTable();
	_CuckooHashTable(const _CuckooHashTable&);
	~_CuckooHashTable();

	const _CuckooHashTable &operator=(const _CuckooHashTable&);

	void insert(const T&);
	void remove(const T&);

	bool contains(const T&) const;
	const T *lookup(const T&) const;

	size_t size() const;
	bool is_empty() const;

	std::vector<const T*> items() const;
};


template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable() :
	_CuckooHashTable(INITIAL_SIZE)
{
}

template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable(size_t size) :
	h_1(H(size)),
	h_2(H(size))
{
	this->T_1 = new Table<T>(size);
	this->T_2 = new Table<T>(size);
	this->L = new NodeList<T>();

	for(size_t i = 0; i < this->T_1->size(); ++i)
	{
		(*this->T_1)[i] = NULL;
		(*this->T_2)[i] = NULL;
	}
}

template<class T, class H>
_CuckooHashTable<T, H>::_CuckooHashTable(const _CuckooHashTable &h) :
	h_1(h.h_1),
	h_2(h.h_2)
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
	size_t table, index;
	const T *key;
	TableNode<T> *new_node;
	NodeListConstIter<T> it_h_L;
	NodeListIter<T> it_L;

	this->h_1 = h.h_1;
	this->h_2 = h.h_2;

	this->T_1 = new Table<T>(h.T_1->size());
	this->T_2 = new Table<T>(h.T_2->size());
	this->L = new NodeList<T>();

	for(it_h_L = h.L->begin(); it_h_L != h.L->end(); ++it_h_L)
	{
		table = std::get<1>(*it_h_L);
		index = std::get<2>(*it_h_L);
		const Table<T> &h_t = table == 1 ? *h.T_1 : *h.T_2;
		Table<T> &t = table == 1 ? *this->T_1 : *this->T_2;

		key = new T(*std::get<0>(*h_t[index]));

		ListNode<T> list_node {key, table, index};
		this->L->push_back(list_node);
		it_L = this->L->end();
		it_L--;
		new_node = new TableNode<T> {key, it_L};

		t[index] = new_node;
	}
}

template<class T, class H>
void _CuckooHashTable<T, H>::erase()
{
	for(size_t i = 0; i < this->T_1->size(); ++i)
	{
		if( (*this->T_1)[i] != NULL )
			delete std::get<0>(*(*this->T_1)[i]);
		if( (*this->T_2)[i] != NULL )
			delete std::get<0>(*(*this->T_2)[i]);

		delete (*this->T_1)[i];
		delete (*this->T_2)[i];
	}

	delete this->L;
	delete this->T_1;
	delete this->T_2;
}

template<class T, class H>
double _CuckooHashTable<T, H>::load_factor() const
{
	return double(this->size()) / (2*this->T_1->size());
}

template<class T, class H>
size_t _CuckooHashTable<T, H>::new_table_size() const
{
	return 2 * this->T_1->size();
}

template<class T, class H>
void _CuckooHashTable<T, H>::rehash(bool should_resize)
{
	size_t new_table_size = should_resize ?
				this->new_table_size() :
				this->size();

	// Temporary table to rehash the keys on.
	_CuckooHashTable new_table(new_table_size);

	NodeListIter<T> it;

	for( it = this->L->begin(); it != this->L->end(); it++ )
	{
		ListNode<T> list_node = *it;
		const T *key = std::get<0>(list_node);
		new_table.insert(*key);
	}

	// Swap table pointers and copy hash functions in order to use the rehashed
	// table from now on.
	std::swap(this->T_1, new_table.T_1);
	std::swap(this->T_2, new_table.T_2);
	std::swap(this->L, new_table.L);
	this->h_1 = new_table.h_1;
	this->h_2 = new_table.h_2;
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
	const T *value = new T(key);

	// First check if the key is already stored. If so, update its value
	// and stop.
	if( this->set_if_present(value, key) )
		return;

	bool inserted = false;

	while( !inserted )
	{
		for(int i = 0; i < MAX_LOOP; ++i)
		{
			// Place current key in T_1. If no one is kicked out, we are done.
			if( (value = this->insert_in(value, 1)) == NULL )
			{
				inserted = true;
				break;
			}

			// Otherwise, repeat for the new evicted key and T_2.
			if( (value = this->insert_in(value, 2)) == NULL )
			{
				inserted = true;
				break;
			}
		}

		// MAX_LOOP exceeded. Renew hash functions and rehash the table.
		if( !inserted )
			// No need to resize as the load factor did not increase.
			this->rehash(false);
	}
}

template<class T, class H>
const T *_CuckooHashTable<T, H>::insert_in(const T *value, size_t table)
{
	const T *evicted = NULL;

	H &hasher = table == 1 ? this->h_1 : this->h_2;
	Table<T> &t = table == 1 ? *this->T_1 : *this->T_2;

	size_t index = hasher.hash(*value);

	if( t[index] != NULL )
		evicted = this->evict(table, index);

	t[index] = this->new_node(value, table, index);

	return evicted;
}

template<class T, class H>
const T *_CuckooHashTable<T, H>::evict(size_t table, size_t index)
{
	Table<T> &t = table == 1 ? *this->T_1 : *this->T_2;
	NodeListIter<T> it;

	const T *evicted = new T(*std::get<0>(*t[index]));
	it = std::get<1>(*t[index]);

	delete std::get<0>(*t[index]);
	delete t[index];
	this->L->erase(it);

	return evicted;
}

template<class T, class H>
TableNode<T> *_CuckooHashTable<T, H>::new_node(const T *key, size_t table, size_t index)
{
	ListNode<T> list_node {key, table, index};

	this->L->push_back(list_node);
	NodeListIter<T> it = this->L->end();

	return new TableNode<T> {key, --it};
}

template<class T, class H>
void _CuckooHashTable<T, H>::remove(const T& key)
{
	this->set_if_present(NULL, key);
}

template<class T, class H>
bool _CuckooHashTable<T, H>::set_if_present(const T *value, const T& key)
{
	// Update values of already placed keys. Can be useful for
	// types having satellite data (such as dictionary key-value pairs).
	const TableNode<T> *node = this->lookup_node(key);
	if( node != NULL )
	{
		NodeListIter<T> it = std::get<1>(*node);

		ListNode<T> list_node = *it;
		size_t table = std::get<1>(list_node);
		size_t index = std::get<2>(list_node);
		Table<T> &t = table == 1 ? *this->T_1 : *this->T_2;

		TableNode<T> *new_node = NULL;
		if( value != NULL )
			new_node = this->new_node(value, table, index);

		delete std::get<0>(*t[index]);
		delete t[index];
		this->L->erase(it);

		t[index]= new_node;
	}

	return node != NULL;
}

template<class T, class H>
bool _CuckooHashTable<T, H>::contains(const T& key) const
{
	return this->lookup_node(key) != NULL;
}

template<class T, class H>
const T *_CuckooHashTable<T, H>::lookup(const T& key) const
{
	const TableNode<T> *node = this->lookup_node(key);
	return node == NULL ? NULL : std::get<0>(*node);
}

template<class T, class H>
const TableNode<T> *_CuckooHashTable<T, H>::lookup_node(const T& key) const
{
	size_t h1 = this->h_1.hash(key);
	if( (*this->T_1)[h1] && *std::get<0>(*(*this->T_1)[h1]) == key )
		return (*this->T_1)[h1];

	size_t h2 = this->h_2.hash(key);
	if( (*this->T_2)[h2] && *std::get<0>(*(*this->T_2)[h2]) == key )
		return (*this->T_2)[h2];

	return NULL;
}

template<class T, class H>
bool _CuckooHashTable<T, H>::is_empty() const
{
	return this->size() == 0;
}

template<class T, class H>
size_t _CuckooHashTable<T, H>::size() const
{
	return this->L->size();
}

template<class T, class H>
std::vector<const T*> _CuckooHashTable<T, H>::items() const
{
	// Returns an array holding the keys stored in the table
	// without any particular order.

	std::vector<const T*> items;

	NodeListConstIter<T> it;

	for(it = this->L->begin(); it != this->L->end(); it++ )
	{
		const T *key = std::get<0>(*it);
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
		this->copy_from(h);
	}

	return *this;
}

// Type alias for cuckoo hash tables that use the standard hashing function.
template<class T>
using CuckooHashTable = _CuckooHashTable<T, Hasher>;

#endif
