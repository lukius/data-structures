#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "xfast.h"

using namespace std;


class XFastTrieTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		this->T = new XFastTrie(7);
		XFastTrie &T = *this->T;

		TrieNode *root = new TrieNode;
		TrieNode *node01 = new TrieNode;
		TrieNode *node02 = new TrieNode;
		TrieNode *node13 = new TrieNode;
		TrieNode *node41 = new TrieNode;
		TrieNode *node42 = new TrieNode;
		TrieNode *node62 = new TrieNode;
		TrieNode *node53 = new TrieNode;
		TrieNode *node63 = new TrieNode;
		TrieNode *node73 = new TrieNode;

		T.root = root;

		root->right = node41;
		root->left = node01;

		node01->left = node02;
		node01->succ = node53;

		node02->succ = node13;

		node41->right = node62;
		node41->left = node42;

		node42->right = node53;
		node42->pred = node13;

		node62->right = node73;
		node62->left = node63;

		node13->is_leaf = true;
		node13->value = 1;

		node53->is_leaf = true;
		node53->value = 5;

		node63->is_leaf = true;
		node63->value = 6;

		node73->is_leaf = true;
		node73->value = 7;

		node13->next = node53;
		node53->next = node63;
		node63->next = node73;
		node53->prev = node13;
		node63->prev = node53;
		node73->prev = node63;

		T.insert_prefix(1, node01, 1);
		T.insert_prefix(1, node02, 2);
		T.insert_prefix(1, node13, 3);
		T.insert_prefix(5, node41, 1);
		T.insert_prefix(5, node42, 2);
		T.insert_prefix(5, node53, 3);
		T.insert_prefix(7, node62, 2);
		T.insert_prefix(7, node73, 3);
		T.insert_prefix(6, node63, 3);

		nodes.push_back(root);
		nodes.push_back(node01);
		nodes.push_back(node02);
		nodes.push_back(node13);
		nodes.push_back(node41);
		nodes.push_back(node42);
		nodes.push_back(node62);
		nodes.push_back(node53);
		nodes.push_back(node63);
		nodes.push_back(node73);
	}

	virtual void TearDown()
	{
		for(size_t i = 0; i < this->nodes.size(); ++i)
			delete nodes[i];

		delete this->T;
	}

	XFastTrie *T;
	vector<TrieNode*> nodes;
};

TEST_F(XFastTrieTest, digits_test)
{
	XFastTrie T(9);

	list<int> *digits = T.binary_digits(5);
	vector<int> expected({0, 1, 0, 1});
	list<int>::iterator it = digits->begin();

	for(int i = 0; it != digits->end(); ++it, ++i)
		EXPECT_EQ(*it, expected[i]);

	delete digits;
}

TEST_F(XFastTrieTest, prefix_search_test)
{
	TrieNode *node_found = this->T->search_longest_prefix_index(5);
	EXPECT_EQ(node_found->value, 5);

	node_found = this->T->search_longest_prefix_index(0);
	EXPECT_FALSE(node_found->is_leaf);
}

TEST_F(XFastTrieTest, leaf_successor_test)
{
	EXPECT_EQ(this->T->successor(5), 6);
	EXPECT_EQ(this->T->successor(6), 7);
}

TEST_F(XFastTrieTest, right_thread_successor_test)
{
	EXPECT_EQ(this->T->successor(0), 1);
	EXPECT_EQ(this->T->successor(2), 5);
}

TEST_F(XFastTrieTest, left_thread_successor_test)
{
	EXPECT_EQ(this->T->successor(4), 5);
}
