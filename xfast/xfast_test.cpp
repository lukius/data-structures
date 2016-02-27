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

TEST(XFastTrieTest, prefix_test)
{
	XFastTrie T(7);

	list<int> *digits = T.binary_digits(5);
	vector<int> *prefixes = T.prefixes(*digits);
	vector<int> expected({4, 4, 5});

	for(size_t i = 0; i < prefixes->size(); ++i)
		EXPECT_EQ((*prefixes)[i], expected[i]);

	delete digits;
	delete prefixes;
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

	list<int> *digits = T.binary_digits(5);
	vector<int> *prefixes = T.prefixes(*digits);
	TrieNode *node_found = T.search_longest_prefix_index(*prefixes);

	EXPECT_EQ(node_found, &node53);

	delete digits;
	delete prefixes;

	digits = T.binary_digits(1);
	prefixes = T.prefixes(*digits);
	node_found = T.search_longest_prefix_index(*prefixes);

	EXPECT_EQ(node_found, null);

	delete digits;
	delete prefixes;
}
