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

		TrieNode *node01 = new TrieNode;
		TrieNode *node02 = new TrieNode;
		TrieNode *node13 = new TrieNode;
		TrieNode *node41 = new TrieNode;
		TrieNode *node42 = new TrieNode;
		TrieNode *node62 = new TrieNode;
		TrieNode *node53 = new TrieNode;
		TrieNode *node63 = new TrieNode;
		TrieNode *node73 = new TrieNode;
		TrieNode *root = T.root;

		root->children[1] = node41;
		root->children[0] = node01;

		node01->children[0] = node02;
		node01->succ = node53;

		node02->children[1] = node13;
		node02->succ = node13;

		node41->children[1] = node62;
		node41->children[0] = node42;

		node42->children[1] = node53;
		node42->pred = node13;

		node62->children[1] = node73;
		node62->children[0] = node63;

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
	}

	virtual void TearDown()
	{
		delete this->T;
	}

	XFastTrie *T;
};

TEST_F(XFastTrieTest, digits_test)
{
	XFastTrie T(9);

	vector<int> *digits = T.binary_digits(5);
	vector<int> expected({0, 1, 0, 1});
	vector<int>::iterator it = digits->begin();

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

TEST_F(XFastTrieTest, insertion_test)
{
	XFastTrie T(15);

	EXPECT_TRUE(T.is_empty());

	T.insert(7);

	EXPECT_FALSE(T.is_empty());
	EXPECT_TRUE(T.contains(7));
	EXPECT_FALSE(T.contains(6));
	EXPECT_FALSE(T.contains(8));
	EXPECT_EQ(T.get_min(), 7);
	EXPECT_EQ(T.get_max(), 7);
	EXPECT_EQ(T.successor(2), 7);
	EXPECT_EQ(T.predecessor(10), 7);

	T.insert(6);

	EXPECT_TRUE(T.contains(7));
	EXPECT_TRUE(T.contains(6));
	EXPECT_FALSE(T.contains(8));
	EXPECT_EQ(T.get_min(), 6);
	EXPECT_EQ(T.get_max(), 7);
	EXPECT_EQ(T.successor(6), 7);
	EXPECT_EQ(T.predecessor(7), 6);

	T.insert(9);

	EXPECT_TRUE(T.contains(7));
	EXPECT_TRUE(T.contains(6));
	EXPECT_TRUE(T.contains(9));
	EXPECT_FALSE(T.contains(8));
	EXPECT_EQ(T.get_min(), 6);
	EXPECT_EQ(T.get_max(), 9);
	EXPECT_EQ(T.successor(6), 7);
	EXPECT_EQ(T.successor(7), 9);
	EXPECT_EQ(T.successor(8), 9);
	EXPECT_EQ(T.predecessor(9), 7);
	EXPECT_EQ(T.predecessor(8), 7);
	EXPECT_EQ(T.predecessor(7), 6);

	T.insert(8);

	EXPECT_TRUE(T.contains(7));
	EXPECT_TRUE(T.contains(6));
	EXPECT_TRUE(T.contains(9));
	EXPECT_TRUE(T.contains(8));
	EXPECT_EQ(T.get_min(), 6);
	EXPECT_EQ(T.get_max(), 9);
	EXPECT_EQ(T.successor(6), 7);
	EXPECT_EQ(T.successor(7), 8);
	EXPECT_EQ(T.successor(8), 9);
	EXPECT_EQ(T.predecessor(9), 8);
	EXPECT_EQ(T.predecessor(8), 7);
	EXPECT_EQ(T.predecessor(7), 6);
}

TEST_F(XFastTrieTest, deletion_test)
{
	XFastTrie T(15);

	EXPECT_TRUE(T.is_empty());

	T.insert(7);

	EXPECT_FALSE(T.is_empty());
	EXPECT_TRUE(T.contains(7));

	// Remove a complete branch
	T.remove(7);

	EXPECT_TRUE(T.is_empty());
	EXPECT_FALSE(T.contains(7));

	T.insert(7);
	T.insert(4);
	// Remove part of a branch (leave shared prefix nodes)
	T.remove(7);

	EXPECT_FALSE(T.is_empty());
	EXPECT_FALSE(T.contains(7));
	EXPECT_TRUE(T.contains(4));
	EXPECT_EQ(T.get_min(), 4);
	EXPECT_EQ(T.get_max(), 4);

	T.insert(15);
	T.insert(0);
	T.remove(0);

	EXPECT_FALSE(T.is_empty());
	EXPECT_FALSE(T.contains(0));
	EXPECT_TRUE(T.contains(4));
	EXPECT_TRUE(T.contains(15));
	EXPECT_EQ(T.get_min(), 4);
	EXPECT_EQ(T.get_max(), 15);
	EXPECT_EQ(T.successor(4), 15);
	EXPECT_EQ(T.successor(10), 15);
	EXPECT_EQ(T.predecessor(15), 4);
	EXPECT_EQ(T.predecessor(10), 4);

	T.remove(4);

	EXPECT_FALSE(T.is_empty());
	EXPECT_FALSE(T.contains(0));
	EXPECT_FALSE(T.contains(4));
	EXPECT_TRUE(T.contains(15));
	EXPECT_EQ(T.get_min(), 15);
	EXPECT_EQ(T.get_max(), 15);
	EXPECT_EQ(T.successor(4), 15);
	EXPECT_EQ(T.successor(10), 15);

	T.remove(15);

	EXPECT_TRUE(T.is_empty());
	EXPECT_FALSE(T.contains(0));
	EXPECT_FALSE(T.contains(4));
	EXPECT_FALSE(T.contains(15));
}

TEST_F(XFastTrieTest, copy_test)
{
	XFastTrie T(15), S(7);

	T.insert(2);
	T.insert(14);
	T.insert(15);
	T.insert(6);

	S.insert(1);
	S.insert(3);

	S = T;

	EXPECT_FALSE(S.is_empty());
	EXPECT_TRUE(S.contains(2));
	EXPECT_TRUE(S.contains(14));
	EXPECT_TRUE(S.contains(15));
	EXPECT_TRUE(S.contains(6));
	EXPECT_FALSE(S.contains(1));
	EXPECT_FALSE(S.contains(3));

	XFastTrie U(S);

	EXPECT_FALSE(U.is_empty());
	EXPECT_TRUE(U.contains(2));
	EXPECT_TRUE(U.contains(14));
	EXPECT_TRUE(U.contains(15));
	EXPECT_TRUE(U.contains(6));
	EXPECT_FALSE(U.contains(1));
	EXPECT_FALSE(U.contains(3));
}
