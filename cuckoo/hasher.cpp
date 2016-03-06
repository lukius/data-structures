#include "hasher.h"
#include <random>
#include <math.h>

#define W (std::numeric_limits<ulong>::max())


Hasher::Hasher(ulong w) : w(w)
{
	std::random_device rd;

	this->mt = new std::mt19937(rd());
	this->d = new std::uniform_int_distribution<ulong>(1,  W-1);

	int M = std::numeric_limits<int>::max();
	std::uniform_int_distribution<int> d(1,  M);
	ulong n = d(*this->mt);
	this->q = (ulong)LOG(W,2) - (ulong)LOG(n,2);

	this->update();
}

Hasher::Hasher(const Hasher &h)
{
	this->copy_from(h);
}

Hasher::~Hasher()
{
	this->erase();
}

void Hasher::erase()
{
	delete this->mt;
	delete this->d;
}

void Hasher::copy_from(const Hasher &h)
{
	std::random_device rd;
	this->mt = new std::mt19937(rd());
	this->d = new std::uniform_int_distribution<ulong>(1, W-1);

	this->w = h.w;
	this->q = h.q;
	this->a1 = h.a1;
	this->a2 = h.a2;
	this->a3 = h.a3;
}

void Hasher::update()
{
	this->a1 = this->rand_odd();
	this->a2 = this->rand_odd();
	this->a3 = this->rand_odd();
}

ulong Hasher::rand_odd() const
{
	ulong n = (*this->d)(*this->mt);
	if( (n&0x1) == 0 )
		n++;

	return n;
}

ulong Hasher::hash_with(ulong a, ulong x) const
{
	x *= a;
	x >>= this->q;
	return x;
}

size_t Hasher::hash(ulong key) const
{
    ulong h1 = this->hash_with(this->a1, key);
    ulong h2 = this->hash_with(this->a2, key);
    ulong h3 = this->hash_with(this->a3, key);

    return (h1 ^ h2 ^ h3) % this->w;
}

size_t Hasher::hash(const std::string &str) const
{
	return this->hash(str.c_str(), str.size());
}

size_t Hasher::hash(const char *bytes, size_t size) const
{
	// See comment on hasher.h

	size_t h = 0;

	for( size_t i = 0; i < size; ++i )
		h += this->hash((ulong)bytes[i]);

	return this->hash(h);
}

const Hasher& Hasher::operator=(const Hasher& h)
{
	if( this != &h )
	{
		this->erase();
		this->copy_from(h);
	}

	return *this;
}
