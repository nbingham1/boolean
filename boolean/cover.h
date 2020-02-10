/*
 * cover.h
 *
 *  Created on: Sep 15, 2014
 *      Author: nbingham
 */

#include <common/standard.h>
#include "cube.h"

#ifndef boolean_cover_h
#define boolean_cover_h

using std::vector;
using std::ostream;

namespace boolean
{
struct cover
{
	cover();
	cover(int val);
	cover(int uid, int val);
	cover(cube s);
	cover(std::vector<cube> s);
	~cover();

	vector<cube> cubes;

	int size() const;
	void reserve(int s);
	void push_back(const cube &s);
	void pop_back();
	cube &back();
	void insert(vector<cube>::iterator position, vector<cube>::iterator first, vector<cube>::iterator last);
	void erase(vector<cube>::iterator first, vector<cube>::iterator last);
	void clear();
	vector<cube>::iterator begin();
	vector<cube>::iterator end();

	bool is_subset_of(const cube &s) const;
	bool is_subset_of(const cover &s) const;
	bool is_tautology() const;
	bool is_null() const;
	int area() const;

	vector<int> vars() const;
	void vars(vector<int> *result) const;
	cover refactor(vector<pair<int, int> > uids);
	cover remote(vector<vector<int> > groups);

	cube supercube() const;

	cube mask();
	cover mask(int v);
	cover mask(cube m);
	cover flipped_mask(cube m);
	void hide(int uid);
	void hide(vector<int> uids);
	void cofactor(const cube &s2);
	void cofactor(int uid, int val);
	float partition(cover &left, cover &right);

	void espresso();
	void minimize();

	cover &operator=(cover c);
	cover &operator=(cube c);
	cover &operator=(int val);

	cover &operator&=(cover c);
	cover &operator&=(cube c);
	cover &operator&=(int val);

	cover &operator|=(cover c);
	cover &operator|=(cube c);
	cover &operator|=(int val);

	cube &at(int i);
	const cube &at(int i) const;
	cube &operator[](int i);
	const cube &operator[](int i) const;

	void hash(hasher &hash) const;
};

ostream &operator<<(ostream &os, cover m);

// Logic Minimization
void espresso(cover &F, const cover &D, const cover &R);
pair<int, int> get_cost(cover &F);
void expand(cover &F, const cover &R, const cube &always);
vector<pair<unsigned int, int> > weights(cover &F);
cube essential(cover &F, const cover &R, int c, const cube &always);
cube feasible(cover &F, const cover &R, int c, const cube &free);
bool guided(cover &F, int c, const cube &free);
void reduce(cover &F);
void irredundant(cover &F);

bool mergible(const cover &c1, const cover &c2);

cover local_assign(const cover &s1, const cube &s2, bool stable);
cover local_assign(const cube &s1, const cover &s2, bool stable);
cover local_assign(const cover &s1, const cover &s2, bool stable);
cover remote_assign(const cover &s1, const cube &s2, bool stable);
cover remote_assign(const cube &s1, const cover &s2, bool stable);
cover remote_assign(const cover &s1, const cover &s2, bool stable);
int passes_guard(const cube &encoding, const cube &global, const cover &guard, cube *total = NULL);
bool violates_constraint(const cube &global, const cover &mutex);
vector<int> passes_constraint(const cover &global, const cover &mutex);
bool vacuous_assign(const cube &encoding, const cover &assignment, bool stable);

cover operator~(cover s1);

cover merge_complement_a1(int uid, const cover &s0, const cover &s1, const cover &F);
cover merge_complement_a2(int uid, const cover &s0, const cover &s1, const cover &F);

cover operator&(cover s1, cover s2);
cover operator&(cover s1, cube s2);
cover operator&(cube s1, cover s2);
cover operator&(cover s1, int s2);
cover operator&(int s1, cover s2);

bool are_mutex(const cover &s1, const cube &s2);
bool are_mutex(const cover &s1, const cover &s2);
bool are_mutex(const cover &s1, const cover &s2, const cover &s3);
bool are_mutex(const cover &s1, const cover &s2, const cover &s3, const cover &s4);

cover operator|(cover s1, cover s2);
cover operator|(cover s1, cube s2);
cover operator|(cube s1, cover s2);
cover operator|(cover s1, int s2);
cover operator|(int s1, cover s2);

cover cofactor(const cover &s1, const cube &s2);
cover cofactor(const cover &s1, int uid, int val);

cube supercube_of_complement(const cover &s);

bool operator==(const cover &s1, const cover &s2);
bool operator==(const cover &s1, const cube &s2);
bool operator==(const cube &s1, const cover &s2);
bool operator==(cover s1, int s2);
bool operator==(int s1, cover s2);

bool operator!=(const cover &s1, const cover &s2);
bool operator!=(const cover &s1, const cube &s2);
bool operator!=(const cube &s1, const cover &s2);
bool operator!=(cover s1, int s2);
bool operator!=(int s1, cover s2);

}

#endif
