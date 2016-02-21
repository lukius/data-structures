#include <string>
#include "gtest/gtest.h"
#include "cuckoo.h"
#include "hasher.h"

using namespace std;

#define DEFAULT_W 25
#define DEFAULT_Q 21


class NonRandomHasher : public Hasher
{
public:
	NonRandomHasher(ulong w) : Hasher(w)
	{
		this->q = DEFAULT_Q;
		this->update();
	};

	void update()
	{
		this->a1 = 3;
		this->a2 = 5;
		this->a3 = 7;
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
	ulong x = 1000;
	ulong n = hasher.hash(x);

	// 187 = [(3 * x) mod (2^25)] div 2^{25 - 21}
	// 312 = [(5 * x) mod (2^25)] div 2^{25 - 21}
	// 437 = [(7 * x) mod (2^25)] div 2^{25 - 21}
	EXPECT_EQ(n, 187 ^ 312 ^ 437);
}

TEST(HasherTest, str_hash_test)
{
	NonRandomHasher hasher = NonRandomHasher(DEFAULT_W);
	string s("asdf");
	ulong n = hasher.hash(s);

	// hash('a') = 38
	// hash('s') = 4
	// hash('d') = 38
	// hash('f') = 32
	// => hash(38+4+38+32)
	EXPECT_EQ(n, 21 ^ 35 ^ 49);
}

TEST_F(CuckooHashTableTest, initialization_test)
{
	EXPECT_TRUE(this->T->is_empty());
}

TEST_F(CuckooHashTableTest, basic_insertion_test)
{
	this->T->insert(10);

	EXPECT_FALSE(this->T->is_empty());
	EXPECT_TRUE(this->T->contains(10));
}

TEST_F(CuckooHashTableTest, insertion_test_on_basic_kick_out)
{
	// TODO
}

