#ifndef _X_FAST_H_
#define _X_FAST_H_

#include "cuckoo.h"
#include "gtest/gtest_prod.h"
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

	std::vector<int> *binary_digits(int) const;

	TrieNode *lookup_prefix(int, size_t) const;
	TrieNode *search_longest_prefix_index(int) const;
	TrieNode *successor_node(int) const;
	TrieNode *predecessor_node(int) const;
	TrieNode *new_leaf_node(int, TrieNode*, TrieNode*);

	void insert_prefix(int, TrieNode*, size_t);

	// Friend test cases (ugly)
	friend class XFastTrieTest;
	FRIEND_TEST(XFastTrieTest, digits_test);
	FRIEND_TEST(XFastTrieTest, prefix_search_test);
	FRIEND_TEST(XFastTrieTest, leaf_successor_test);
	FRIEND_TEST(XFastTrieTest, right_thread_successor_test);
	FRIEND_TEST(XFastTrieTest, left_thread_successor_test);

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
