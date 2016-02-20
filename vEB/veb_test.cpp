#include "veb.h"
#include "gtest/gtest.h"

using namespace std;


class vEBTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		this->t = new vEBTree(77);
	}

	virtual void TearDown()
	{
		delete this->t;
	}

	vEBTree *t;
};


TEST_F(vEBTest, initialization_test)
{
	EXPECT_TRUE(this->t->is_empty());
}

TEST_F(vEBTest, insertion_test_1)
{
	this->t->insert(5);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 5);
}

TEST_F(vEBTest, insertion_test_2)
{
	this->t->insert(5);
	this->t->insert(27);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 27);
	EXPECT_EQ(this->t->successor(5), 27);
	EXPECT_EQ(this->t->predecessor(27), 5);
}

TEST_F(vEBTest, insertion_test_3)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(this->t->contains(16));
	EXPECT_TRUE(this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 27);
	EXPECT_EQ(this->t->successor(5), 16);
	EXPECT_EQ(this->t->successor(16), 27);
	EXPECT_EQ(this->t->predecessor(27), 16);
}

TEST_F(vEBTest, insertion_test_4)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);
	this->t->insert(15);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(this->t->contains(15));
	EXPECT_TRUE(this->t->contains(16));
	EXPECT_TRUE(this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 27);
	EXPECT_EQ(this->t->successor(5), 15);
	EXPECT_EQ(this->t->successor(15), 16);
	EXPECT_EQ(this->t->successor(16), 27);
	EXPECT_EQ(this->t->predecessor(16), 15);
	EXPECT_EQ(this->t->predecessor(27), 16);
}

TEST_F(vEBTest, deletion_test_1)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);
	this->t->insert(15);
	this->t->erase(27);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(this->t->contains(15));
	EXPECT_TRUE(this->t->contains(16));
	EXPECT_TRUE(!this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 16);
	EXPECT_EQ(this->t->successor(5), 15);
	EXPECT_EQ(this->t->successor(15), 16);
	EXPECT_EQ(this->t->predecessor(15), 5);
}

TEST_F(vEBTest, deletion_test_2)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);
	this->t->insert(15);
	this->t->erase(27);
	this->t->erase(15);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(this->t->contains(5));
	EXPECT_TRUE(!this->t->contains(15));
	EXPECT_TRUE(this->t->contains(16));
	EXPECT_TRUE(!this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 5);
	EXPECT_EQ(this->t->get_max(), 16);
	EXPECT_EQ(this->t->successor(1), 5);
	EXPECT_EQ(this->t->successor(5), 16);
	EXPECT_EQ(this->t->successor(10), 16);
	EXPECT_EQ(this->t->predecessor(16), 5);
	EXPECT_EQ(this->t->predecessor(10), 5);
	EXPECT_EQ(this->t->predecessor(50), 16);
}

TEST_F(vEBTest, deletion_test_3)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);
	this->t->insert(15);
	this->t->erase(27);
	this->t->erase(15);
	this->t->erase(5);

	EXPECT_TRUE(!this->t->is_empty());
	EXPECT_TRUE(!this->t->contains(5));
	EXPECT_TRUE(!this->t->contains(15));
	EXPECT_TRUE(this->t->contains(16));
	EXPECT_TRUE(!this->t->contains(27));
	EXPECT_TRUE(!this->t->contains(10));
	EXPECT_EQ(this->t->get_min(), 16);
	EXPECT_EQ(this->t->get_max(), 16);
	EXPECT_EQ(this->t->successor(10), 16);
	EXPECT_EQ(this->t->predecessor(50), 16);
}

TEST_F(vEBTest, deletion_test_4)
{
	this->t->insert(5);
	this->t->insert(27);
	this->t->insert(16);
	this->t->insert(15);
	this->t->erase(27);
	this->t->erase(15);
	this->t->erase(5);
	this->t->erase(16);

	EXPECT_TRUE(this->t->is_empty());
}
