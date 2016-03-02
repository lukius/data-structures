#ifndef _XFAST_HASHER_H_
#define _XFAST_HASHER_H_

#include <vector>
#include "hasher.h"
#include "cuckoo.h"

struct TrieNode
{
	// Left and right nodes indexed 0 and 1, respectively
	std::vector<TrieNode*> children;

	// Links to predecessor and successor leaves, which are meaningful
	// only if left or right are null, respectively
	TrieNode *pred, *succ;

	// Only meaningful for leaf nodes
	TrieNode *prev, *next;
	int value;

	bool is_leaf;

	TrieNode();
};

struct XFastTableNode
{
	int first;
	TrieNode *second;

	XFastTableNode(int, TrieNode*);

	bool operator==(const XFastTableNode&) const;
};


class XFastHasher : public Hasher
{
public:

	XFastHasher(ulong);

	size_t hash(const XFastTableNode &) const;
};


class XFastTable : public _CuckooHashTable<XFastTableNode, XFastHasher>
{
public:

	TrieNode *lookup(int) const;

	void remove(int);
};

#endif
