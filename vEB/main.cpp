#include "veb.h"
#include <assert.h>
using namespace std;


int main(int argc, char *argv[])
{
    vEBTree t(77);
    assert(t.is_empty());

    t.insert(5);
    assert(!t.is_empty());
    assert(t.contains(5));
    assert(!t.contains(10));
    assert(t.get_min() == 5);
    assert(t.get_max() == 5);

    t.insert(27);
    assert(!t.is_empty());
    assert(t.contains(5));
    assert(t.contains(27));
    assert(!t.contains(10));
    assert(t.get_min() == 5);
    assert(t.get_max() == 27);
    /*
    assert(t.successor(5) == 27);
    assert(t.predecessor(27) == 5);
    
    t.insert(16);
    assert(!t.is_empty());
    assert(t.contains(5));
    assert(t.contains(16));
    assert(t.contains(27));
    assert(!t.contains(10));
    assert(t.get_min() == 5);
    assert(t.get_max() == 27);
    assert(t.successor(5) == 16);
    assert(t.successor(16) == 27);
    assert(t.predecessor(27) == 16);

    t.insert(15);
    assert(!t.is_empty());
    assert(t.contains(5));
    assert(t.contains(16));
    assert(t.contains(15));
    assert(t.contains(27));
    assert(!t.contains(10));
    assert(t.get_min() == 5);
    assert(t.get_max() == 27);
    assert(t.successor(5) == 15);
    assert(t.successor(15) == 16);
    assert(t.successor(16) == 27);
    assert(t.predecessor(27) == 16);

    t.erase(27);
    assert(!t.is_empty());
    assert(t.contains(5));
    assert(t.contains(15));
    assert(t.contains(16));
    assert(!t.contains(27));
    assert(!t.contains(10));
    assert(t.get_min() == 5);
    assert(t.get_max() == 16);
    assert(t.successor(5) == 15);
    assert(t.successor(15) == 16);
    assert(t.predecessor(15) == 5);

    t.erase(5);
    assert(!t.is_empty());
    assert(!t.contains(5));
    assert(t.contains(15));
    assert(t.contains(16));
    assert(!t.contains(27));
    assert(!t.contains(10));
    assert(t.get_min() == 15);
    assert(t.get_max() == 16);

    t.erase(15);
    t.erase(16);
    assert(t.is_empty());
    */
}
