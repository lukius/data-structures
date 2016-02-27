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

	T.hash_tables[1].insert(4);
	T.hash_tables[2].insert(4);
	T.hash_tables[2].insert(5);
	T.hash_tables[3].insert(5);
	T.hash_tables[3].insert(6);
	T.hash_tables[3].insert(7);

	list<int> *digits = T.binary_digits(5);
	vector<int> *prefixes = T.prefixes(*digits);
	size_t index = T.search_longest_prefix_index(*prefixes);

	EXPECT_EQ(index, 3);

	delete digits;
	delete prefixes;

	digits = T.binary_digits(1);
	prefixes = T.prefixes(*digits);
	index = T.search_longest_prefix_index(*prefixes);

	EXPECT_EQ(index, 0);

	delete digits;
	delete prefixes;
}
