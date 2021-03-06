#include "xfast.h"
#include "xfast_table.h"
#include "cuckoo.h"
#include <stack>
#include <cmath>
#include <unordered_map>
#include <assert.h>
#include <stddef.h>

using namespace std;

#define N_BITS(n)	(size_t)ceil((std::log(n+1)/std::log(2)))
#define PREFIX(x,i)	((x >> (this->n-i)) << (this->n-i))


XFastTrie::XFastTrie(size_t U) : U(U)
{
	this->n = N_BITS(U);

	this->hash_tables.resize(this->n + 1);

	this->root = new TrieNode();
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

	TrieNode *current, *leaf_node, *previous, *successor, *predecessor;

	bool should_delete_node = true, previous_node_deleted;

	// Iterate nodes in a bottom-up fashion. Keep deleting them until we
	// find one that has the other branch active. We stop there since
	// the branch is still needed by other values in the data structure.
	for(ssize_t i = this->n; i >= 0; --i)
	{
		// When i = 0, we have reached the root of the trie. Otherwise,
		// there must be a node holding the i-th prefix of the value.
		current = i == 0 ? this->root : this->lookup_prefix(value, i);

		// Ensure that the trie's root is never deleted.
		should_delete_node = should_delete_node && (i != 0);

		if(static_cast<size_t>(i) == this->n)
		{
			// Leaf node. Reconnect neighbors in the linked list.
			leaf_node = current;
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
				if(previous_node_deleted)
				{
					current->children[0] = NULL;
					if(current->children[1] != NULL)
					{
						// This node should survive as its right child still
						// holds data. Since the left child is no longer
						// present, we must set the successor pointer to the
						// lowest value in the right subtree, which by
						// definition is the successor of the value being
						// removed.
						current->succ = successor;
						should_delete_node = false;
					}
				}

				// Update node predecessor: the highest value in the left tree
				// is now the predecessor of the value being removed.
				if(current->pred == leaf_node)
					current->pred =  predecessor;
			}
			else if(current->children[1] == previous)
			{
				// Analogous to previous case.
				if(previous_node_deleted)
				{
					current->children[1] = NULL;
					if(current->children[0] != NULL)
					{
						current->pred = predecessor;
						should_delete_node = false;
					}
				}

				if(current->succ == leaf_node)
					current->succ =  successor;
			}
		}

		// Non-root nodes should be deleted and their prefixes removed from the
		// hash tables.
		if(should_delete_node)
		{
			this->remove_prefix(value, i);
			previous_node_deleted = true;
			// Since top level nodes might need to have their successors/
			// predecessors adjusted, depending on whether they point to the
			// leaf node or not, we delay the actual deletion of the
			// leaf node.
			if(static_cast<size_t>(i) != this->n)
				delete current;
		}
		else
			previous_node_deleted = false;

		previous = current;
	}

	delete leaf_node;
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

// Copy constructor, destructor, copy assignment operator and helper methods.

XFastTrie::XFastTrie(const XFastTrie &t)
{
	this->copy_from(t);
}

XFastTrie::~XFastTrie()
{
	this->erase();
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

	this->hash_tables.clear();
}

void XFastTrie::copy_from(const XFastTrie &t)
{
	// As this is a complex data structure, the copy algorithm works in three
	// phases:
	//  1. Traverse the trie and create new nodes with the same structure.
	//     Map each original node to the one that was created in a dictionary.
	//  2. Update predecessors/successors and linked leaf nodes. For this,
	//     traverse the new trie and use the dictionary to map the pointers.
	//  3. Update hash tables, inserting each prefix at a time and using again
	//     the dictionary to insert proper node pointers.

	unordered_map<const TrieNode*, TrieNode*> node_map;

	this->n = t.n;
	this->U = t.U;

	this->copy_trie(t.root, &this->root, node_map);
	this->update_trie(node_map);
	this->update_hash_tables(t, node_map);
}

void XFastTrie::copy_trie(const TrieNode *source,
		TrieNode **dest,
		unordered_map<const TrieNode*, TrieNode*> &node_map)
{
	TrieNode *new_node = new TrieNode();

	new_node->prev = source->prev;
	new_node->next = source->next;
	new_node->pred = source->pred;
	new_node->succ = source->succ;
	new_node->is_leaf = source->is_leaf;
	new_node->value = source->value;

	if(source->children[0] != NULL)
		this->copy_trie(source->children[0], &new_node->children[0], node_map);

	if(source->children[1] != NULL)
		this->copy_trie(source->children[1], &new_node->children[1], node_map);

	*dest = new_node;

	node_map.insert({source, new_node});
}

void XFastTrie::update_trie(const unordered_map<const TrieNode*, TrieNode*> &node_map)
{
	stack<TrieNode*> nodes;
	TrieNode *current;
	unordered_map<const TrieNode*, TrieNode*>::const_iterator it;

	nodes.push(this->root);

	while(!nodes.empty())
	{
		current = nodes.top();
		nodes.pop();
		if(current->children[0] != NULL)
			nodes.push(current->children[0]);
		if(current->children[1] != NULL)
			nodes.push(current->children[1]);

		if(current->pred != NULL)
		{
			it = node_map.find(current->pred);
			assert(it != node_map.end());
			current->pred = it->second;
		}

		if(current->succ != NULL)
		{
			it = node_map.find(current->succ);
			assert(it != node_map.end());
			current->succ = it->second;
		}

		if(current->prev != NULL)
		{
			it = node_map.find(current->prev);
			assert(it != node_map.end());
			current->prev = it->second;
		}

		if(current->next != NULL)
		{
			it = node_map.find(current->next);
			assert(it != node_map.end());
			current->next = it->second;
		}
	}
}

void XFastTrie::update_hash_tables(const XFastTrie &t,
		const unordered_map<const TrieNode*, TrieNode*> &node_map)
{
	int prefix;
	vector<const XFastTableNode*> items;
	unordered_map<const TrieNode*, TrieNode*>::const_iterator it;

	this->hash_tables.resize(t.hash_tables.size());

	for(size_t i = 0; i < this->hash_tables.size(); ++i)
	{
		// TODO: implement iterators to improve efficiency.
		items = t.hash_tables[i].items();
		for(size_t j = 0; j < items.size(); ++j)
		{
			prefix = items[j]->first;
			it = node_map.find(items[j]->second);
			assert(it != node_map.end());
			this->hash_tables[i].insert({prefix, it->second});
		}
	}
}
