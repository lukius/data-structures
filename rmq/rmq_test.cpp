#include <vector>
#include "gtest/gtest.h"
#include "rmq.h"

using namespace std;


class RMQTest : public testing::Test {};

vector<int> A({45, 53, 22, 14, 17, 28, -10, 23, 72, 82});

TEST(RMQTest, naive_rmq_test)
{
	NaiveRMQ<int> naive_rmq(A);

	EXPECT_EQ(naive_rmq(0,9), -10);
	EXPECT_EQ(naive_rmq(2,2), 22);
	EXPECT_EQ(naive_rmq(1,4), 14);
	EXPECT_EQ(naive_rmq(7,9), 23);
}

TEST(RMQTest, fully_precomputed_rmq_test)
{
	FullyPrecomputedRMQ<int> full_rmq(A);

	EXPECT_EQ(full_rmq(0,9), -10);
	EXPECT_EQ(full_rmq(2,2), 22);
	EXPECT_EQ(full_rmq(1,4), 14);
	EXPECT_EQ(full_rmq(7,9), 23);
}

TEST(RMQTest, block_rmq_test)
{
	BlockRMQ<int> block_rmq(A);

	EXPECT_EQ(block_rmq(0,9), -10);
	EXPECT_EQ(block_rmq(2,2), 22);
	EXPECT_EQ(block_rmq(1,4), 14);
	EXPECT_EQ(block_rmq(7,9), 23);
}

TEST(RMQTest, sparse_rmq_test)
{
	SparseTableRMQ<int> sparse_rmq(A);

	EXPECT_EQ(sparse_rmq(0,9), -10);
	EXPECT_EQ(sparse_rmq(2,2), 22);
	EXPECT_EQ(sparse_rmq(1,4), 14);
	EXPECT_EQ(sparse_rmq(7,9), 23);
}
