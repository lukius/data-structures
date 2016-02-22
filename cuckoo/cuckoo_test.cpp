#include <string>
#include <algorithm>
#include "gtest/gtest.h"
#include "cuckoo.h"
#include "hasher.h"

using namespace std;

#define DEFAULT_W 32


class NonRandomHasher : public Hasher
{
public:
	NonRandomHasher(ulong w) : Hasher(w)
	{
		this->q = 1;
		this->update();
	};

	void update()
	{
		this->a1 = min((int)this->w - 1, 3);
		this->a2 = min((int)this->w - 1, 5);
		this->a3 = min((int)this->w - 1, 7);
	}
};

class CuckooHashTableTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		this->T = new _CuckooHashTable<int, NonRandomHasher>();
	}

	virtual void TearDown()
	{
		delete this->T;
	}

	_CuckooHashTable<int, NonRandomHasher> *T;
};


TEST(HasherTest, int_hash_test)
{
	NonRandomHasher hasher = NonRandomHasher(DEFAULT_W);
	ulong x = 100;
	ulong n = hasher.hash(x);

	// 6 = [(3 * x) mod (2^32)] div 2^{5 - 4}
	// 10 = [(5 * x) mod (2^32)] div 2^{5 - 4}
	// 14 = [(7 * x) mod (2^32)] div 2^{5 - 4}
	EXPECT_EQ(n, 6 ^ 10 ^ 14);
}

TEST(HasherTest, str_hash_test)
{
	NonRandomHasher hasher = NonRandomHasher(DEFAULT_W);
	string s("asdf");
	ulong n = hasher.hash(s);

	// hash('a') = 0
	// hash('s') = 1
	// hash('d') = 2
	// hash('f') = 3
	// => hash(0+1+2+3)
	EXPECT_EQ(n, 3);
}

TEST_F(CuckooHashTableTest, initialization_test)
{
	EXPECT_TRUE(this->T->is_empty());
}

TEST_F(CuckooHashTableTest, basic_insertion_test)
{
	this->T->insert(1000);

	EXPECT_FALSE(this->T->is_empty());
	EXPECT_TRUE(this->T->contains(1000));
	EXPECT_FALSE(this->T->contains(10));
}

TEST_F(CuckooHashTableTest, insertion_with_one_kickout_test)
{
	// hash(1000) = hash(892) = 0
	this->T->insert(1000);
	this->T->insert(892);

	EXPECT_FALSE(this->T->is_empty());
	EXPECT_TRUE(this->T->contains(1000));
	EXPECT_TRUE(this->T->contains(892));
}

TEST_F(CuckooHashTableTest, rehashing_test)
{
	this->T->insert(1000);
	this->T->insert(892);
	// This will trigger a reshash since the load factor is above the
	// threshold.
	this->T->insert(984);

	EXPECT_FALSE(this->T->is_empty());
	EXPECT_TRUE(this->T->contains(1000));
	EXPECT_TRUE(this->T->contains(892));
	EXPECT_TRUE(this->T->contains(984));
}

TEST_F(CuckooHashTableTest, deletion_test)
{
	this->T->insert(1000);
	this->T->insert(892);
	this->T->insert(984);

	this->T->remove(892);
	this->T->remove(984);

	EXPECT_FALSE(this->T->is_empty());
	EXPECT_TRUE(this->T->contains(1000));
	EXPECT_FALSE(this->T->contains(892));
	EXPECT_FALSE(this->T->contains(984));

	this->T->remove(1000);

	EXPECT_TRUE(this->T->is_empty());
}

TEST_F(CuckooHashTableTest, several_insertions_and_deletions_test)
{
	for(int n = 0; n < 1000; n++)
	{
		this->T->insert(n);
		EXPECT_EQ(this->T->size(), n+1);
	}

	EXPECT_FALSE(this->T->is_empty());

	for(int n = 0; n < 1000; n++)
		EXPECT_TRUE(this->T->contains(n));

	size_t size = this->T->size();
	for(int n = 500; n < 1000; n++)
	{
		this->T->remove(n);
		size--;
		EXPECT_EQ(this->T->size(), size);
	}

	for(int n = 0; n < 1000; n++)
		EXPECT_TRUE(n < 500 ? this->T->contains(n) : !this->T->contains(n));

	for(int n = 0; n < 1000; n++)
		this->T->insert(n);

	EXPECT_EQ(this->T->size(), 1000);

	for(int n = 0; n < 1000; n++)
		EXPECT_TRUE(this->T->contains(n));

	_CuckooHashTable<int, NonRandomHasher> S;
	S = *this->T;

	EXPECT_EQ(S.size(), 1000);

	for(int n = 0; n < 1000; n++)
		EXPECT_TRUE(S.contains(n));
}

TEST_F(CuckooHashTableTest, items_test)
{
	for(int n = 0; n < 1000; n++)
		this->T->insert(n);

	vector<const int*> items = this->T->items();

	EXPECT_EQ(items.size(), 1000);

	for(int i = 0; i < 1000; i++)
	{
		EXPECT_GE(*items[i], 0);
		EXPECT_LT(*items[i], 1000);
	}
}
