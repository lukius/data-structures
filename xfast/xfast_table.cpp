#include <tuple>
#include "xfast_table.h"

XFastTableNode::XFastTableNode(int first, TrieNode *second) :
	first(first),
	second(second)
{
}

bool XFastTableNode::operator==(const XFastTableNode& node) const
{
	return this->first == node.first;
}

XFastHasher::XFastHasher(ulong w) : Hasher(w)
{
}

size_t XFastHasher::hash(const XFastTableNode &node) const
{
	return Hasher::hash(node.first);
}

TrieNode *XFastTable::lookup(int value) const
{
	TrieNode dummy_trie_node;
	XFastTableNode dummy_node {value, &dummy_trie_node};
	const XFastTableNode *node = _CuckooHashTable<XFastTableNode, XFastHasher>::lookup(dummy_node);
	return node == NULL ? NULL : node->second;
}
