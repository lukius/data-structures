#include "xfast.h"
#include "cuckoo.h"
#include <list>
#include <cmath>
#include <assert.h>
#include <stddef.h>

using namespace std;

#define N_BITS(n)		(size_t)ceil((std::log(n+1)/std::log(2)))
#define PREFIX(x, i)	((x >> (this->n-i)) << (this->n-i))


XFastTrie::XFastTrie(size_t U) : U(U)
{
	this->n = N_BITS(U);

	this->hash_tables.resize(this->n + 1);

	this->root = new TrieNode();
}

XFastTrie::XFastTrie(const XFastTrie &t)
{
	// TODO
}

XFastTrie::~XFastTrie()
{
	// TODO
}

void XFastTrie::erase()
{
	// TODO
}

void XFastTrie::copy_from(const XFastTrie &t)
{
	// TODO
}

void XFastTrie::insert(int value)
{
	// TODO
}

void XFastTrie::remove(int value)
{
	// TODO
}

bool XFastTrie::contains(int value) const
{
	// Lookup takes constant time: just check the cuckoo hash table in the
	// bottom level.
	return this->hash_tables[this->n].lookup(value) != NULL;
}

int XFastTrie::get_min() const
{
	// The minimum element is 0 or the successor of 0.

	if(this->contains(0))
		return 0;

	return this->successor(0);
}

int XFastTrie::get_max() const
{
	// Similarly, the maximum is either U or U's predecessor.

	if(this->contains(this->U))
		return this->U;

	return this->predecessor(this->U);
}

bool XFastTrie::is_empty() const
{
	return
		this->root->left == NULL &&
		this->root->right == NULL;
}

int XFastTrie::successor(int value) const
{
	assert(!this->is_empty() && value < this->U);

	TrieNode *node = this->search_longest_prefix_index(value);

	if(node->is_leaf)
	{
		assert(node->next != NULL);
		return node->next->value;
	}

	if(node->succ != NULL)
		return node->succ->value;

	// Must have predecessor pointer
	return node->pred->next->value;
}

int XFastTrie::predecessor(int value) const
{
	// Finding the predecessor follows the same logic as finding the successor.

	assert(!this->is_empty() && value > 0);

	TrieNode *node = this->search_longest_prefix_index(value);

	if(node->is_leaf)
	{
		assert(node->prev != NULL);
		return node->prev->value;
	}

	if(node->pred != NULL)
		return node->pred->value;

	return node->succ->prev->value;
}

list<int> *XFastTrie::binary_digits(int value) const
{
	list<int> *digits = new list<int>();
	div_t dv {};

	while(value > 0)
	{
		dv = div(value, 2);
		digits->push_front(dv.rem);
		value = dv.quot;
	}

	// Left-pad with zeroes.
	while(digits->size() < this->n)
		digits->push_front(0);

	return digits;
}

TrieNode *XFastTrie::lookup_prefix(int value, size_t i) const
{
	int prefix = PREFIX(value, i);
	return this->hash_tables[i].lookup(prefix);
}

void XFastTrie::insert_prefix(int value, TrieNode *node, size_t i)
{
	int prefix = PREFIX(value, i);
	XFastTableNode table_node {prefix, node};
	this->hash_tables[i].insert(table_node);
}

TrieNode *XFastTrie::search_longest_prefix_index(int value) const
{
	// Search for the highest-indexed hash table containing a prefix.

 	if(this->lookup_prefix(value, 1) == NULL)
		return NULL;

 	// k is the index of the last level where a prefix was found.
	size_t i = 1, j = this->n, m, k = 1;

	while(i <= j)
	{
		m = (i + j) >> 1;
		if(this->lookup_prefix(value, m) != NULL)
		{
			i = m + 1;
			k = m;
		}
		else
			j = m - 1;
	}

	return this->lookup_prefix(value, k);
}

const XFastTrie &XFastTrie::operator=(const XFastTrie& t)
{
	if(this != &t)
	{
		this->erase();
		this->copy_from(t);
	}

	return *this;
}
