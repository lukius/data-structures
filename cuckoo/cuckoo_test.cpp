#include <string>
#include "gtest/gtest.h"
#include "hasher.h"

using namespace std;


class NonRandomHasher : public Hasher
{
public:
	NonRandomHasher(ulong w, ulong q) : Hasher(w,q)
	{
		this->update();
	};

	void update()
	{
		this->a1 = 3;
		this->a2 = 5;
		this->a3 = 7;
	}
};


TEST(HasherTest, int_hash_test)
{
	NonRandomHasher hasher = NonRandomHasher(25, 21);
	ulong x = 1000;
	ulong n = hasher.hash(x);

	// 187 = [(3 * x) mod (2^25)] div 2^{25 - 21}
	// 312 = [(5 * x) mod (2^25)] div 2^{25 - 21}
	// 437 = [(7 * x) mod (2^25)] div 2^{25 - 21}
	EXPECT_EQ(n, 187 ^ 312 ^ 437);
}

TEST(HasherTest, str_hash_test)
{
	NonRandomHasher hasher = NonRandomHasher(25, 21);
	std::string s("asdf");
	ulong n = hasher.hash(s);

	// hash('a') = 38
	// hash('s') = 4
	// hash('d') = 38
	// hash('f') = 32
	// => hash(38+4+38+32)
	EXPECT_EQ(n, 21 ^ 35 ^ 49);
}
