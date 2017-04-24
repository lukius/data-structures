#include "mmheap.h"
#include "gtest/gtest.h"

using namespace std;


class MinMaxHeapTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		this->t = new MinMaxHeap<int>();
	}

	virtual void TearDown()
	{
		delete this->t;
	}

	MinMaxHeap<int> *t;
};


TEST_F(MinMaxHeapTest, initialization_test)
{
	EXPECT_TRUE(this->t->is_empty());
}

TEST_F(MinMaxHeapTest, insertion_test_1)
{
	this->t->insert(5);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_EQ(this->t->peek_min(), 5);
	EXPECT_EQ(this->t->peek_max(), 5);
}

TEST_F(MinMaxHeapTest, insertion_test_2)
{
	this->t->insert(5);
	this->t->insert(3);

	EXPECT_EQ(this->t->size(), 2);
	EXPECT_EQ(this->t->peek_min(), 3);
	EXPECT_EQ(this->t->peek_max(), 5);
}

TEST_F(MinMaxHeapTest, extraction_test_1)
{
	this->t->insert(5);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_EQ(this->t->extract_min(), 5);
	EXPECT_TRUE(this->t->is_empty());
}

TEST_F(MinMaxHeapTest, extraction_test_2)
{
	this->t->insert(5);
	this->t->insert(3);

	EXPECT_EQ(this->t->size(), 2);
	EXPECT_EQ(this->t->extract_max(), 5);
	EXPECT_EQ(this->t->size(), 1);
	EXPECT_EQ(this->t->extract_max(), 3);
	EXPECT_TRUE(this->t->is_empty());
}

TEST_F(MinMaxHeapTest, combined_test_1)
{
	this->t->insert(5);
	this->t->insert(3);
	this->t->insert(-2);
	this->t->insert(6);
	this->t->insert(11);
	this->t->insert(0);
	this->t->insert(8);
	this->t->insert(20);
	this->t->insert(-3);

	EXPECT_EQ(this->t->size(), 9);
	EXPECT_EQ(this->t->peek_min(), -3);
	EXPECT_EQ(this->t->peek_max(), 20);

	// Should extract -3.
	this->t->extract_min();
	// Should extract -2.
	this->t->extract_min();

	EXPECT_EQ(this->t->size(), 7);
	EXPECT_EQ(this->t->peek_min(), 0);
	EXPECT_EQ(this->t->peek_max(), 20);

	// Should extract 20.
	this->t->extract_max();
	this->t->insert(19);

	EXPECT_EQ(this->t->size(), 7);
	EXPECT_EQ(this->t->peek_min(), 0);
	EXPECT_EQ(this->t->peek_max(), 19);

	// Should extract 0.
	this->t->extract_min();
	// Should extract 19.
	this->t->extract_max();
	// Should extract 3.
	this->t->extract_min();
	// Should extract 11.
	this->t->extract_max();

	EXPECT_EQ(this->t->size(), 3);
	EXPECT_EQ(this->t->peek_min(), 5);
	EXPECT_EQ(this->t->peek_max(), 8);
}

TEST_F(MinMaxHeapTest, combined_test_2)
{
	int N(2048);

	for(int i = 0; i < N; ++i)
	{
		this->t->insert(i);
		EXPECT_EQ(this->t->size(), i+1);
		EXPECT_EQ(this->t->peek_min(), 0);
		EXPECT_EQ(this->t->peek_max(), i);
	}

	for(int i = 1; i <= 100; ++i)
	{
		EXPECT_EQ(this->t->extract_min(), i-1);
		EXPECT_EQ(this->t->peek_max(), N-1);
	}

	for(int i = N; i < 2*N; ++i)
	{
		this->t->insert(i);
		EXPECT_EQ(this->t->peek_min(), 100);
		EXPECT_EQ(this->t->peek_max(), i);
	}

	for(int i = 1; i <= 100; ++i)
	{
		EXPECT_EQ(this->t->extract_max(), 2*N-i);
		EXPECT_EQ(this->t->peek_min(), 100);
	}
}

TEST_F(MinMaxHeapTest, from_vector_test)
{
	MinMaxHeap<int> h({1,3,4,12,0,-1,7,9,2});

	EXPECT_EQ(h.size(), 9);
	EXPECT_EQ(h.extract_min(), -1);
	EXPECT_EQ(h.extract_max(), 12);

	h.insert(6);
	h.insert(20);

	EXPECT_EQ(h.extract_min(), 0);
	EXPECT_EQ(h.extract_max(), 20);
}

TEST_F(MinMaxHeapTest, copy_test)
{
	this->t->insert(5);
	this->t->insert(4);
	this->t->insert(3);

	MinMaxHeap<int> h1(*this->t), h2;

	EXPECT_EQ(h1.size(), 3);
	EXPECT_EQ(h1.extract_min(), 3);
	EXPECT_EQ(h1.extract_max(), 5);

	h2 = *this->t;

	EXPECT_EQ(h2.size(), 3);
	EXPECT_EQ(h2.extract_min(), 3);
	EXPECT_EQ(h2.extract_max(), 5);
}
