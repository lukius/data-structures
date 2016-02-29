#ifndef _X_FAST_H_
#define _X_FAST_H_

#include "cuckoo.h"
#include "gtest/gtest_prod.h"
#include <list>
#include <vector>
#include <stddef.h>
#include "xfast_table.h"


class XFastTrie
{
private:

	// Universe size
	int U;
	// Bit size of the integers in the range [0, U]
	size_t n;

	// Hash tables (one per layer)
	std::vector<XFastTable> hash_tables;

	// Root node of the trie
	TrieNode *root;

	void erase();
	void copy_from(const XFastTrie&);

	std::list<int> *binary_digits(int) const;
	TrieNode *lookup_prefix(int, size_t) const;
	TrieNode *search_longest_prefix_index(int) const;

	// Friend test cases (ugly)
	FRIEND_TEST(XFastTrieTest, digits_test);
	FRIEND_TEST(XFastTrieTest, prefix_search_test);

public:
	XFastTrie(size_t);
	XFastTrie(const XFastTrie&);
	~XFastTrie();

    void insert(int);
    void remove(int);

    int get_min() const;
    int get_max() const;
    bool is_empty() const;
    bool contains(int) const;
    int successor(int) const;
    int predecessor(int) const;

	const XFastTrie &operator=(const XFastTrie&);
};

#endif
