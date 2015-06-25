#include "veb.h"
#include <vector>
#include <assert.h>
#include <math.h>
#include <stddef.h>

using namespace std;


vEBTree::vEBTree(int n)
{
    this->block_size = n > 2 ? (size_t)floor(sqrt(n)) : n;
    size_t num_children = n > 2 ? (size_t)ceil(sqrt(n)) : 0;

    this->initialize_children(num_children);
    this->initialize_summary(num_children);

    this->min = NULL;
    this->max = NULL;
}

void vEBTree::initialize_children(size_t num_children) 
{
    this->children.resize(num_children);
    for(size_t i = 0; i < num_children; ++i)
        this->children[i] = new vEBTree((int)this->block_size);
}

void vEBTree::initialize_summary(size_t num_children) 
{
    this->summary = NULL;
    if(num_children > 0)
        this->summary = new vEBTree(num_children);
}

size_t vEBTree::child_index(int value) const
{
    return value / this->block_size;
}

int vEBTree::child_value(int value) const
{
    return value % this->block_size;
}

void vEBTree::_insert(int value)
{
    // Recursively insert the given value into the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    if(this->summary && this->children[index]->is_empty())
        this->summary->insert(index);
    this->children[index]->insert(child_value);
}

void vEBTree::_erase(int value)
{
    // Recursively remove the given value from the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    this->children[index]->erase(child_value);
    if(this->children[index]->is_empty())
        this->summary->erase(index);
}

bool vEBTree::_contains(int value) const
{
    // Recursively check whether the given value is contained into
    // the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    return this->children[index]->contains(child_value);
}

void vEBTree::insert(int value)
{
    // Case 1 (trivial): insert first value.
    if(this->is_empty())
    {
        this->min = new int(value);
        this->max = new int(value);
        return;
    }

    // Case 2: insert a new minimum.
    if(value <= *this->min)
    {
        // Important: recursively insert the former minimum into
        // the tree, only if it is different from the maximum.
        if(*this->min != *this->max)
            this->_insert(*this->min);
        *this->min = value;
        return;
    }

    // Case 3: insert a new maximum.
    if(value >= *this->max)
    {
        if(*this->max != *this->min)
            this->_insert(*this->max);
        *this->max = value;
        return;
    }

    // Case 4: insert any other value recursively.
    this->_insert(value);
}

void vEBTree::erase(int value)
{
    // Precondition: tree must not be empty.
    // Also, value should be stored in the tree.
    assert(!this->is_empty());

    // Trivial case: tree has only one element.
    if(value == *this->min && value == *this->max)
    {
        delete this->min;
        delete this->max;
        this->min = NULL;
        this->max = NULL;
        return;
    }

    // Delete the minimum. For this, we have to find the successor.
    // It is important not to call successor(value) in order to
    // keep O(log log n) time complexity (doing that will instead
    // increase this bound to O(log n)).
    if(value == *this->min)
    {
        int new_min;

        // These cases are analogous to subcases of case 2 of
        // the successor method.
        if(this->summary->is_empty())
            new_min = *this->max;
        else
        {
            new_min = (*this->summary->min * this->block_size) +
                      *this->children[*this->summary->min]->min;
            // It is important to erase this value from the tree, 
            // since it will be now stored separately.
            this->_erase(new_min);
        }
    
        // Finally, update minimum and return.
        *this->min = new_min;
        return;
    }

    // Delete the maximum. Analogous to previous case (comments on
    /// predecessor method might also help).
    if(value == *this->max)
    {
        int new_max;

        if(this->summary->is_empty())
            new_max = *this->min;
        else
        {
            new_max = (*this->summary->max * this->block_size) +
                      *this->children[*this->summary->max]->max;
            this->_erase(new_max);
        }
    
        *this->max = new_max;
        return;
    }

    // Erase any other value recursively.
    this->_erase(value);
}

bool vEBTree::contains(int value) const
{
    if(this->is_empty())
        return false;

    if(value < *this->min || value > *this->max)
        return false;

    if(value == *this->min || value == *this->max)
        return true;

    return this->_contains(value);
}

bool vEBTree::is_empty() const
{
    return this->min == NULL;
}


int vEBTree::successor(int value) const
{
    // Precondition: tree must not be empty and the value has to
    // be less than the maximum currently stored.
    assert(!this->is_empty() && value < *this->max);

    // Case 1: the value is less than the minimum (trivial case).
    if(value < *this->min)
        return *this->min;

    size_t index = this->child_index(value);
    int child_value = this->child_value(value),
        offset = value - child_value;

    // Seek the successor of the minimum.
    if(value == *this->min)
    {
        // Case 2a: tree contains no other value except from min and max.
        if(this->summary->is_empty())
            return *this->max;
        // Case 2b: tree contains additional values. Thus, the answer is
        // the minimum value stored in the minimum block. We have to be
        // careful and add the appropriate offset in order to give the
        // correct answer.
        else
            return (*this->summary->min * this->block_size) +
                   *this->children[*this->summary->min]->min;
    }

    // Search the successor of any value != min.
    // Case 3a: the successor exists in the same block.
    if(this->children[index]->max && child_value < *this->children[index]->max)
        return offset + this->children[index]->successor(child_value);
    // Case 3b: the successor appears in the next nonempty block.
    else if(this->summary->max && (int)index < *this->summary->max)
    {
        int successor_block = this->summary->successor(index);
        return offset + this->block_size +
               *this->children[successor_block]->min;        
    }
    // Case 3c: no nonempty blocks remaining. Return max.
    else return *this->max;
}

int vEBTree::predecessor(int value) const
{
    // See comments of previous method (they are analogous).

    assert(!this->is_empty() && value > *this->min);

    if(value > *this->max)
        return *this->max;	
    
    size_t index = this->child_index(value);
    int child_value = this->child_value(value),
        offset = value - child_value;

    if(value == *this->max)
    {
        if(this->summary->is_empty())
            return *this->min;
        else
            return (*this->summary->max * this->block_size) +
                   *this->children[*this->summary->max]->max;
    }

    if(this->children[index]->min && child_value > *this->children[index]->min)
        return offset + this->children[index]->predecessor(child_value);
    else if(this->summary->min && (int)index > *this->summary->min)
    {
        int predecessor_block = this->summary->predecessor(index);
        return offset - this->block_size +
               *this->children[predecessor_block]->max;        
    }
    else return *this->min;
}

int vEBTree::get_min() const
{
    assert(!this->is_empty());

    return *this->min;
}

int vEBTree::get_max() const
{
    assert(!this->is_empty());

    return *this->max;
}
