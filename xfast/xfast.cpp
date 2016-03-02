#include "xfast.h"
#include "cuckoo.h"
#include <stack>
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
	this->copy_from(t);
}

XFastTrie::~XFastTrie()
{
	this->erase();
}

void XFastTrie::erase()
{
	stack<TrieNode*> nodes;

	nodes.push(this->root);

	while(!nodes.empty())
	{
		TrieNode *node = nodes.top();
		nodes.pop();
		if(node->children[0] != NULL)
			nodes.push(node->children[0]);
		if(node->children[1] != NULL)
			nodes.push(node->children[1]);
		delete node;
	}

}

void XFastTrie::copy_from(const XFastTrie &t)
{
	// TODO
}

// Expected O(log U) time complexity since insertion in cuckoo hash tables
// takes expected O(1) time.
void XFastTrie::insert(int value)
{
	if(this->contains(value))
		return;

	vector<int> &digits = *this->binary_digits(value);

	TrieNode *successor_node = this->successor_node(value);
	TrieNode *predecessor_node = this->predecessor_node(value);
	TrieNode *leaf_node = this->new_leaf_node(value, successor_node, predecessor_node);

	TrieNode *current = this->root;

	// Iterate through the digits and update the trie.
	for(size_t i = 0; i < this->n - 1; ++i)
	{
		int digit = digits[i];

		if(current->children[digit] != NULL)
		{
			// Node already exists. Just update successor and predecessor
			// pointers: as this node's successor is greater than the newly
			// inserted value, by definition this value now becomes its
			// successor.
			if(current->succ != NULL && current->succ == successor_node)
				current->succ = leaf_node;
			if(current->pred != NULL && current->pred == predecessor_node)
				current->pred = leaf_node;

			current = current->children[digit];
		}
		else
		{
			// Node does not exist. We must create it.
			TrieNode *new_node = new TrieNode();

			// Set successor/predecessor according to the next digit to come.
			// A 1 means that the left pointer will remain null, and so the
			// successor must point to the lowest value in the rightmost tree,
			// which is the new value being inserted.
			if(digits[i+1] == 1)
				new_node->succ = leaf_node;
			if(digits[i+1] == 0)
				new_node->pred = leaf_node;

			// Connect parent and adjust its successor/predecessor pointers.
			current->children[digit] = new_node;
			if(digit == 0)
			{
				current->succ = NULL;
				// TODO: current->pred != NULL && value < current->pred->value
				if(current->pred == NULL && current->children[1] == NULL)
					current->pred = leaf_node;
			}
			if(digit == 1)
			{
				current->pred = NULL;
				if(current->succ == NULL && current->children[0] == NULL)
					current->succ = leaf_node;
			}

			current = new_node;
		}

		// Finally, update the hash table controlling the prefixes in level i+1.
		this->insert_prefix(value, current, i+1);
	}

	// current points to the last node visited in the loop. Thus, we have to
	// connect it to the leaf node.
	current->children[digits[this->n - 1]] = leaf_node;

	delete &digits;
}

void XFastTrie::remove(int value)
{
	if(!this->contains(value))
		return;

	TrieNode *current, *previous, *successor, *predecessor;

	// Iterate nodes in a bottom-up fashion. Keep deleting them until we
	// find one that has the other branch active. We stop there since
	// the branch is still needed by other values in the data structure.
	for(ssize_t i = this->n; i >= 0; --i)
	{
		// When i = 0, we have reached the root of the trie. Otherwise,
		// there must be a node holding the i-th prefix of the value.
		current = i == 0 ? this->root : this->lookup_prefix(value, i);

		if(static_cast<size_t>(i) == this->n)
		{
			// Leaf node. Reconnect neighbors in the linked list.
			successor = current->next;
			predecessor = current->prev;

			if(predecessor != NULL)
				predecessor->next = successor;
			if(successor != NULL)
				successor->prev = predecessor;
		}
		else
		{
			// Internal node. Clear left or right child, as appropriate, and
			// check whether we should stop. If not, set successor/predecessor
			// pointers accordingly.
			if(current->children[0] == previous)
			{
				current->children[0] = NULL;
				if(current->children[1] != NULL)
				{
					current->succ = successor;
					break;
				}
			}
			else if(current->children[1] == previous)
			{
				current->children[1] = NULL;
				if(current->children[0] != NULL)
				{
					current->pred = predecessor;
					break;
				}
			}
		}

		// Non-root nodes should be deleted and their prefixes removed from the
		// hash tables.
		if(i != 0)
		{
			this->remove_prefix(value, i);
			delete current;
			previous = current;
		}
	}
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
		this->root->children[0] == NULL &&
		this->root->children[1] == NULL;
}

int XFastTrie::successor(int value) const
{
	TrieNode *node = this->successor_node(value);

	assert(node != NULL);

	return node->value;
}

int XFastTrie::predecessor(int value) const
{
	TrieNode *node = this->predecessor_node(value);

	assert(node != NULL);

	return node->value;
}

TrieNode *XFastTrie::new_leaf_node(int value, TrieNode *successor,
		TrieNode *predecessor)
{
	TrieNode *new_node = new TrieNode();

	this->insert_prefix(value, new_node, this->n);

	new_node->value = value;
	new_node->is_leaf = true;
	new_node->next = successor;
	new_node->prev = predecessor;

	if(successor != NULL)
		successor->prev = new_node;
	if(predecessor != NULL)
		predecessor->next = new_node;

	return new_node;
}

TrieNode *XFastTrie::successor_node(int value) const
{
	TrieNode *node = this->search_longest_prefix_index(value);

	if(node->is_leaf)
		return node->next;

	if(node->succ != NULL)
		return node->succ;

	if(node->pred != NULL)
		return node->pred->next;

	return NULL;
}

TrieNode *XFastTrie::predecessor_node(int value) const
{
	// Finding the predecessor follows the same logic as finding the successor.

	TrieNode *node = this->search_longest_prefix_index(value);

	if(node->is_leaf)
		return node->prev;

	if(node->pred != NULL)
		return node->pred;

	if(node->succ != NULL)
		return node->succ->prev;

	return NULL;
}

vector<int> *XFastTrie::binary_digits(int value) const
{
	vector<int> *digits = new vector<int>(this->n);
	div_t dv {};
	ssize_t i = this->n - 1;

	while(value > 0)
	{
		dv = div(value, 2);
		(*digits)[i] = dv.rem;
		value = dv.quot;
		i--;
	}

	// Left-pad with zeroes.
	for(; i > 0; i--)
		(*digits)[i] = 0;

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

void XFastTrie::remove_prefix(int value, size_t i)
{
	int prefix = PREFIX(value, i);
	this->hash_tables[i].remove(prefix);
}

TrieNode *XFastTrie::search_longest_prefix_index(int value) const
{
	// Search for the highest-indexed hash table containing a prefix.

 	if(this->lookup_prefix(value, 1) == NULL)
		return this->root;

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
