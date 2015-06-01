#include "rmq.h"
#include <vector>
#include <assert.h>
using namespace std;


int main(int argc, char *argv[])
{
    vector<int> A({45, 53, 22, 14, 17, 28, -10, 23, 72, 82});

    NaiveRMQ<int> naive_rmq(A);
    FullyPrecomputedRMQ<int> full_rmq(A);
    BlockRMQ<int> block_rmq(A); 

    assert(naive_rmq(0,9) == -10);
    assert(naive_rmq(2,2) == 22);
    assert(naive_rmq(1,4) == 14);
    assert(naive_rmq(7,9) == 23);

    assert(full_rmq(0,9) == -10);
    assert(full_rmq(2,2) == 22);
    assert(full_rmq(1,4) == 14);
    assert(full_rmq(7,9) == 23);

    assert(block_rmq(0,9) == -10);
    assert(block_rmq(2,2) == 22);
    assert(block_rmq(1,4) == 14);
    assert(block_rmq(7,9) == 23);
}
