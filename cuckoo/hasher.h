#ifndef _HASHER_H_
#define _HASHER_H_

#include <random>

typedef unsigned long ulong;

#define LOG(x, b) (std::log(x)/std::log(b))


class Hasher
{
	// Implementation of the 2-universal hash family presented in the paper.

private:
	std::mt19937 *mt;
	std::uniform_int_distribution<> *d;

	ulong rand_odd() const;
	ulong hash_with(ulong, ulong) const;

protected:
	// Just to be able to test this class' behavior.
	ulong w, q, a1, a2, a3;

public:
	Hasher(ulong w);
	~Hasher();

	void update();

	size_t hash(ulong) const;
	// The following is an extension for strings which was not part of the
	// original formulation of the data structure. As such, it might not have
	// good properties and thus the complexity analysis might no be valid in
	// these cases.
	size_t hash(const std::string&) const;
	size_t hash(const char*, size_t) const;

	const Hasher& operator=(const Hasher&);
};

#endif
