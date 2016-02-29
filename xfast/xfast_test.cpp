#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "xfast.h"

using namespace std;


class XFastTrieTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		this->T = new XFastTrie(175);
	}

	virtual void TearDown()
	{
		delete this->T;
	}

	XFastTrie *T;
};

TEST(XFastTrieTest, digits_test)
{
	XFastTrie T(9);

	list<int> *digits = T.binary_digits(5);
	vector<int> expected({0, 1, 0, 1});
	list<int>::iterator it = digits->begin();

	for(int i = 0; it != digits->end(); ++it, ++i)
		EXPECT_EQ(*it, expected[i]);

	delete digits;
}

TEST(XFastTrieTest, prefix_search_test)
{
	XFastTrie T(7);

	TrieNode node41;
	TrieNode node42;
	TrieNode node62;
	TrieNode node53;
	TrieNode node63;
	TrieNode node73;
	TrieNode *null = NULL;

	T.insert_prefix(5, &node41, 1);
	T.insert_prefix(5, &node42, 2);
	T.insert_prefix(5, &node53, 3);
	T.insert_prefix(7, &node62, 2);
	T.insert_prefix(7, &node73, 3);
	T.insert_prefix(6, &node63, 3);

	TrieNode *node_found = T.search_longest_prefix_index(5);
	EXPECT_EQ(node_found, &node53);

	node_found = T.search_longest_prefix_index(1);
	EXPECT_EQ(node_found, null);
}
