#include "xfast.h"
#include "cuckoo.h"
#include <list>
#include <cmath>
#include <stddef.h>

using namespace std;

#define N_BITS(n) (size_t)ceil((std::log(n+1)/std::log(2)))


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
	// TODO
}

int XFastTrie::successor(int value) const
{
	// TODO
}

int XFastTrie::predecessor(int value) const
{
	// TODO
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

vector<int> *XFastTrie::prefixes(const list<int> &digits) const
{
	vector<int> *prefixes = new vector<int>(digits.size());
	int prefix = 0, i = 0, digit;
	int k = 1 << (this->n - 1);

	for(list<int>::const_iterator it = digits.begin(); it != digits.end(); ++it)
	{
		digit = *it;
		prefix += k * digit;
		(*prefixes)[i] = prefix;
 		k >>= 1;
 		i++;
	}

	return prefixes;
}

#include <iostream>
TrieNode *XFastTrie::search_longest_prefix_index(const vector<int> &prefixes) const
{
	// Search for the highest-indexed hash table containing a prefix.

 	if(this->hash_tables[1].lookup(prefixes[0]) == NULL)
		return NULL;

	size_t i = 1, j = this->n, m;

	while(i != j)
	{
		m = (i + j) >> 1;
		if(this->hash_tables[m].lookup(prefixes[m-1]) != NULL)
			i = m + 1;
		else
			j = m - 1;
	}
	return this->hash_tables[i].lookup(prefixes[i-1]);
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
