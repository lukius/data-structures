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
    return value / this->children.size();
}

int vEBTree::child_value(int value) const
{
    return value % this->children.size();
}

void vEBTree::_insert(int value)
{
    // Recursively insert the given value into the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    this->children[index]->insert(child_value);
}

void vEBTree::_erase(int value)
{
    // Recursively remove the given value from the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    this->children[index]->erase(child_value);
}

bool vEBTree::_contains(int value) const
{
    // Recursively check whether the given value is contained into the appropriate child.

    size_t index = this->child_index(value);
    int child_value = this->child_value(value);
    return this->children[index]->contains(child_value);
}

void vEBTree::insert(int value)
{
    if(this->is_empty())
    {
        this->min = new int(value);
        this->max = new int(value);
        return;
    }

    if(value <= *this->min)
    {
        this->_insert(*this->min);
        *this->min = value;
        return;
    }

    if(value >= *this->max)
    {
        this->_insert(*this->max);
        *this->max = value;
        return;
    }

    this->_insert(value);
}

void vEBTree::erase(int value)
{

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

}

int vEBTree::predecessor(int value) const
{

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
