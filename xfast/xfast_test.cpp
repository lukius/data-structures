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

	XFastTableNode tnode41 {4, &node41};
	XFastTableNode tnode42 {4, &node42};
	XFastTableNode tnode62 {6, &node62};
	XFastTableNode tnode53 {5, &node53};
	XFastTableNode tnode63 {6, &node63};
	XFastTableNode tnode73 {7, &node73};

	T.hash_tables[1].insert(tnode41);
	T.hash_tables[2].insert(tnode42);
	T.hash_tables[2].insert(tnode62);
	T.hash_tables[3].insert(tnode53);
	T.hash_tables[3].insert(tnode63);
	T.hash_tables[3].insert(tnode73);

	TrieNode *node_found = T.search_longest_prefix_index(5);

	EXPECT_EQ(node_found, &node53);

	node_found = T.search_longest_prefix_index(1);

	EXPECT_EQ(node_found, null);
}
