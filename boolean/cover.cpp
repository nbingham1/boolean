/*
 * cover.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: nbingham
 */

#include "common.h"
#include "cover.h"
#include "tokenizer.h"
#include "variable_space.h"

cover::cover()
{

}

cover::cover(int val)
{
	if (val == 1)
		cubes.push_back(cube());
}

cover::cover(int uid, int val)
{
	cubes.push_back(cube(uid, val));
}

cover::cover(cube s)
{
	cubes.push_back(s);
}

cover::cover(vector<cube> s)
{
	cubes = s;
}

/**
 * \brief	A constructor that parses a string and uses it to fill the canonical expression with values.
 * \param	s		A string that represents the expression.
 * \param	vars	The variable space used to parse the string.
 */
cover::cover(string s, variable_space &vars, tokenizer *tokens)
{
	if (s == "0")
		return;

	vector<string> t = distribute(demorgan(s, -1, false));
	for (int i = 0; i < (int)t.size(); i++)
	{
		cube c(t[i], vars, tokens);
		if (c != 0)
			cubes.push_back(c);
	}
}

cover::~cover()
{

}

int cover::size() const
{
	return (int)cubes.size();
}

void cover::reserve(int s)
{
	cubes.reserve(s);
}

void cover::push_back(const cube &s)
{
	cubes.push_back(s);
}

void cover::pop_back()
{
	cubes.pop_back();
}

cube &cover::back()
{
	return cubes.back();
}

void cover::insert(vector<cube>::iterator position, vector<cube>::iterator first, vector<cube>::iterator last)
{
	cubes.insert(position, first, last);
}

void cover::erase(vector<cube>::iterator first, vector<cube>::iterator last)
{
	cubes.erase(first, last);
}

void cover::clear()
{
	cubes.clear();
}

vector<cube>::iterator cover::begin()
{
	return cubes.begin();
}

vector<cube>::iterator cover::end()
{
	return cubes.end();
}

bool cover::is_subset_of(const cube &s) const
{
	for (int i = 0; i < (int)cubes.size(); i++)
		if (!cubes[i].is_subset_of(s))
			return false;

	return true;
}

bool cover::is_subset_of(const cover &s) const
{
	for (int i = 0; i < (int)cubes.size(); i++)
		if (!cubes[i].is_subset_of(s))
			return false;

	return true;
}

bool cover::is_tautology() const
{
	// Cover F is empty
	if (size() == 0)
		return false;

	// Cover F includes the universal cube
	int max_width = 0;
	for (int i = 0; i < size(); i++)
	{
		int temp = cubes[i].memory_width();
		// if we found a universal cube
		if (temp == 0)
			return true;

		if (temp > max_width)
			max_width = temp;
	}

	if (max_width <= 8)
	{
		cube result;
		if (max_width < 5)
			result.values.push_back(0xFFFFFFFF << powi(2, max_width));
		else
			result.extendN(powi(2, max_width-5));

		for (int i = 0; i < size(); i++)
			result.supercube(cubes[i].get_cover(max_width));

		return result.is_tautology();
	}
	else
	{
		vector<pair<int, int> > binate_rank;
		for (int i = 0; i < size(); i++)
		{
			if ((int)binate_rank.size() < cubes[i].size()*16)
				binate_rank.resize(cubes[i].size()*16, pair<int, int>(0, 0));

			for (int j = 0; j < cubes[i].size()*16; j++)
			{
				int val = cubes[i].get(j);
				if (val == 0)
					binate_rank[j].first++;
				else if (val == 1)
					binate_rank[j].second++;
			}
		}

		for (int i = 0; i < (int)binate_rank.size(); i++)
		{
			// Column of all zeros
			if (binate_rank[i].first == size() || binate_rank[i].second == size())
				return false;
			else if (binate_rank[i].first > 0 && binate_rank[i].second > 0)
			{
				binate_rank[i].first += binate_rank[i].second;
				binate_rank[i].second = i;
			}

			else
			{
				binate_rank[i].first = 0;
				binate_rank[i].second = i;
			}
		}

		// Select a binate variable
		pair<int, int> uid = *max_element(binate_rank.begin(), binate_rank.end());

		if (uid.first > 0)
		{
			if (!::cofactor(*this, uid.second, 0).is_tautology())
				return false;

			if (!::cofactor(*this, uid.second, 1).is_tautology())
				return false;

			return true;
		}

		// Function is unate
		return false;
	}
}

bool cover::is_null() const
{
	for (int i = 0; i < (int)cubes.size(); i++)
		if (!cubes[i].is_null())
			return false;

	return true;
}

int cover::area() const
{
	int result = 0;
	for (int i = 0; i < size(); i++)
		result += at(i).width();

	return result;
}

vector<int> cover::vars() const
{
	vector<int> result;
	for (int i = 0; i < (int)cubes.size(); i++)
		cubes[i].vars(&result);
	sort(result.begin(), result.end());
	result.resize(unique(result.begin(), result.end()) - result.begin());
	return result;
}

void cover::vars(vector<int> *result) const
{
	for (int i = 0; i < (int)cubes.size(); i++)
		cubes[i].vars(result);
	sort(result->begin(), result->end());
	result->resize(unique(result->begin(), result->end()) - result->begin());
}

cover cover::refactor(vector<pair<int, int> > uids)
{
	cover result;
	result.reserve(cubes.size());

	for (int i = 0; i < (int)cubes.size(); i++)
		result.push_back(cubes[i].refactor(uids));

	return result;
}

cube cover::supercube() const
{
	cube result;
	if (size() > 0)
	{
		result.extendN(cubes[0].size());

		for (int i = size()-1; i != -1; i--)
		{
			if (cubes[i].size() < result.size())
				result.trunk(cubes[i].size());

			for (int j = 0; j < result.size(); j++)
				result.values[j] |= cubes[i].values[j];
		}
	}
	else
		result.values.push_back(0);

	return result;
}

void cover::hide(int uid)
{
	for (int i = 0; i < (int)cubes.size(); i++)
		cubes[i].hide(uid);
}

void cover::hide(vector<int> uids)
{
	for (int i = 0; i < (int)cubes.size(); i++)
		cubes[i].hide(uids);
}

void cover::cofactor(const cube &s1)
{
	int w, r;
	for (w = 0, r = 0; r < size(); r++)
	{
		if (cubes[w].size() < cubes[r].size())
			cubes[w].extendX(cubes[r].size());

		bool valid = true;
		int j = 0;
		for (j = 0; j < cubes[r].size() && j < s1.size() && valid; j++)
		{
			uint32_t b = s1.values[j] & cubes[r].values[j];
			b = ~(b | (b >> 1)) & 0x55555555;

			if (b != 0)
				valid = false;
			else
			{
				uint32_t a = (s1.values[j] ^ (s1.values[j] >> 1)) & 0x55555555;
				a = a | (a << 1);
				cubes[w].values[j] = cubes[r].values[j] | a;
			}
		}

		if (valid)
		{
			if (w != r)
			{
				for (; j < cubes[r].size(); j++)
					cubes[w].values[j] = cubes[r].values[j];

				if (cubes[w].size() > cubes[r].size())
					cubes[w].trunk(cubes[r].size());
			}

			w++;
		}
	}
}

void cover::cofactor(int uid, int val)
{
	int w, r;
	for (w = 0, r = 0; r < size(); r++)
	{
		int cmp = cubes[r].get(uid);
		if (cmp != 1-val)
		{
			if (r != w)
				cubes[w] = cubes[r];

			cubes[w].set(uid, 2);
			w++;
		}
	}

	if (w != r)
		erase(begin() + w, end());
}

void cover::espresso()
{
	::espresso(*this, cover(), ~*this);
}

void cover::minimize()
{
	for (int i = (int)cubes.size()-1; i >= 0; i--)
	{
		if (cubes[i].is_null())
			cubes.erase(cubes.begin() + i);
		else if (cubes[i].is_tautology())
		{
			cubes = vector<cube>(1, cube());
			return;
		}
	}

	bool done = false;
	while (!done)
	{
		done = true;
		for (int i = (int)cubes.size() - 1; i >= 0 ; i--)
			for (int j = i-1; j >= 0; j--)
			{
				triple<int, int, int> distances = merge_distances(cubes[i], cubes[j]);
				if ((distances.first <= 1 && distances.second + distances.third <= 1) ||
					(distances.second == 0 && distances.first - distances.third == 0) ||
					(distances.third == 0 && distances.first - distances.second == 0))
				{
					cubes[i].supercube(cubes[j]);
					cubes.erase(cubes.begin() + j);
					i--;
					done = false;
				}
				else if ((distances.second == 0 && distances.first - distances.third == 1) ||
						 (distances.third == 0 && distances.first - distances.second == 1))
				{
					cubes[i].intersect(cubes[j]);
					cubes[i].xoutnulls();
					cubes.erase(cubes.begin() + j);
					i--;
					done = false;
				}
			}
	}
}

cover &cover::operator=(cover c)
{
	cubes = c.cubes;
	return *this;
}

cover &cover::operator=(cube c)
{
	cubes.clear();
	cubes.push_back(c);
	return *this;
}

cover &cover::operator=(int val)
{
	cubes.clear();
	if (val == 1)
		cubes.push_back(cube());
	return *this;
}

cover &cover::operator&=(cover c)
{
	if (c.is_null() || is_null())
		cubes.clear();
	else
		*this = *this & c;

	return *this;
}

cover &cover::operator&=(cube c)
{
	for (int i = 0; i < (int)cubes.size(); i++)
		cubes[i] &= c;

	return *this;
}

cover &cover::operator&=(int val)
{
	if (val == 0)
		cubes.clear();

	return *this;
}

cover &cover::operator|=(cover c)
{
	cubes.insert(cubes.end(), c.begin(), c.end());

	minimize();

	return *this;
}

cover &cover::operator|=(cube c)
{
	cubes.push_back(c);

	return *this;
}

cover &cover::operator|=(int val)
{
	if (val == 1)
		cubes = vector<cube>(1, cube());

	return *this;
}

cube &cover::at(int i)
{
	return cubes[i];
}

const cube &cover::at(int i) const
{
	return cubes[i];
}

cube &cover::operator[](int i)
{
	return cubes[i];
}

const cube &cover::operator[](int i) const
{
	return cubes[i];
}

void espresso(cover &F, const cover &D, const cover &R)
{
	cube always = R.supercube();
	for (int i = 0; i < always.size(); i++)
		always.values[i] = ~always.values[i];

	expand(F, R, always);
	irredundant(F);
	int cost = F.area(), old_cost;
	do
	{
		reduce(F);
		expand(F, R, always);
		irredundant(F);

		old_cost = cost;
		cost = F.area();
	} while (cost < old_cost);
}

void expand(cover &F, const cover &R, const cube &always)
{
	vector<pair<uint32_t, int> > weight = weights(F);
	sort(weight.begin(), weight.end());

	for (int i = 0; i < (int)weight.size(); i++)
	{
		// TODO any time we expand the cube, we need to mark the newly covered cubes as completely redundant, ignore them throughout the expand step and remove them at the end.

		cube free;
		cube feasible_cube = F[weight[i].second];
		do
		{
			free = essential(F, R, weight[i].second, always);

			while (feasible_cube.size() > 0)
			{
				F[weight[i].second] = feasible_cube;
				free = essential(F, R, weight[i].second, always);
				feasible_cube = feasible(F, R, weight[i].second, free);
			}

			free = essential(F, R, weight[i].second, always);
		} while (guided(F, weight[i].second, free));
	}
}

vector<pair<uint32_t, int> > weights(cover &F)
{
	vector<pair<uint32_t, int> > result;
	result.resize(F.size(), pair<uint32_t, int>(0, 0));

	for (int i = 0; i < F.size(); i++)
	{
		result[i].second = i;

		for (int j = i; j < F.size(); j++)
		{
			uint32_t count = 0;
			int size = min(F[i].size(), F[j].size());
			int k = 0;
			for (; k < size; k++)
				count += count_ones(F[i].values[k] & F[j].values[k]);
			for (; k < F[i].size(); k++)
				count += count_ones(F[i].values[k]);
			for (; k < F[j].size(); k++)
				count += count_ones(F[j].values[k]);

			if (i == j)
				result[i].first += count;
			else
			{
				result[i].first += count;
				result[j].first += count;
			}
		}
	}

	return result;
}

cube essential(cover &F, const cover &R, int c, const cube &always)
{
	cube free;
	free.values.reserve(F[c].size());
	for (int j = 0; j < F[c].size() && j < always.size(); j++)
	{
		/* any part which does not appear in any cube in the inverse may
		 * always be raised in the one we are expanding. So raise those parts
		 * and remove them from the free list.
		 */
		free.values.push_back(~F[c].values[j] & ~always.values[j]);
		F[c].values[j] |= always.values[j];
	}
	for (int j = always.size(); j < F[c].size(); j++)
		free.values.push_back(~F[c].values[j]);

	// Find parts that can never be raised
	for (int j = 0; j < R.size(); j++)
	{
		/* if any cube in the inverse is distance 1 from the cube we are
		 * expanding, then all of the parts of the conflicting variable which
		 * are one in that cube may never be raised in the one we are expanding.
		 */
		int size = min(free.size(), R[j].size());
		int conflict;
		uint32_t mask = 0;
		int count = 0;
		for (int k = 0; k < size && count < 2; k++)
		{
			// AND to get the intersection
			uint32_t a = ~free.values[k] & R[j].values[k];
			// OR to find any pairs that are both 0
			a = (~(a | (a >> 1))) & 0x55555555;

			if (a > 0)
			{
				// Save the location of the conflicting 1's
				mask = (a | (a << 1)) & R[j].values[k];
				conflict = k;

				// count the number of bits set to 1 (derived from Hacker's Delight)
				a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
				a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
				a += (a >> 8);
				a += (a >> 16);
				count += a & 0x0000003F;
			}
		}

		// distance is 1, we need to remove the conflicting 1's from the free set.
		if (count == 1)
			free.values[conflict] &= ~mask;
	}

	return free;
}

cube feasible(cover &F, const cover &R, int c, const cube &free)
{
	cube overexpanded = supercube(F[c], free);

	cover feasibly_covered;
	feasibly_covered.reserve(F.size()-1);
	for (int j = 0; j < F.size(); j++)
	{
		if (j != c && F[j].is_subset_of(overexpanded) && !F[j].is_subset_of(F[c]))
		{
			cube test = supercube(F[j], F[c]);
			if (are_mutex(test, R))
				feasibly_covered.push_back(test);
		}
	}

	int max_cover_count = 0;
	int max_feasible_index = 0;
	for (int j = 0; j < feasibly_covered.size(); j++)
	{
		int cover_count = 0;
		for (int k = 0; k < feasibly_covered.size(); k++)
			if (j != k && feasibly_covered[k].is_subset_of(feasibly_covered[j]))
				cover_count++;

		if (cover_count > max_cover_count)
		{
			max_cover_count = cover_count;
			max_feasible_index = j;
		}
	}

	if (feasibly_covered.size() > 0)
		return feasibly_covered[max_feasible_index];
	else
		return cube();
}

bool guided(cover &F, int c, const cube &free)
{
	cube overexpanded = supercube(F[c], free);

	vector<int> covered;
	covered.reserve(F.size());
	for (int j = 0; j < F.size(); j++)
		if (F[j].is_subset_of(overexpanded))
			covered.push_back(j);

	if (covered.size() > 0)
	{
		int max_covered;
		uint32_t max_covered_mask;
		int max_covered_count = 0;
		for (int j = 0; j < free.size(); j++)
		{
			if (free.values[j] != 0)
			{
				// TODO I'm sure there is a faster way to do this than just scanning all 32 or 64 bits... look at the log2i function for an example
				for (uint32_t k = 1; k > 0; k <<= 1)
				{
					if ((k & free.values[j]) != 0)
					{
						int covered_count = 0;
						for (int l = 0; l < (int)covered.size(); l++)
						{
							if (j >= F[covered[l]].size() || (k & F[covered[l]].values[j]) > 0)
								covered_count++;
						}

						if (covered_count > max_covered_count)
						{
							max_covered = j;
							max_covered_mask = k;
							max_covered_count = covered_count;
						}
					}
				}
			}
		}

		if (max_covered_count > 0)
		{
			F[c].values[max_covered] |= max_covered_mask;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void reduce(cover &F)
{
	if (F.cubes.size() > 0)
	{
		random_shuffle(F.begin(), F.end());

		cube c = F.back();
		F.pop_back();

		for (int i = 0; i < F.size(); i++)
		{
			cube s = F[i];
			F[i] = c & supercube_of_complement(cofactor(F, c));
			c = s;
		}

		F.push_back(c & supercube_of_complement(cofactor(F, c)));
	}
}

void irredundant(cover &F)
{
	if (F.cubes.size() > 1)
	{
		cover relatively_essential;
		vector<int> relatively_redundant;
		relatively_essential.reserve(F.size());
		relatively_redundant.reserve(F.size());

		cover test = F;
		test.pop_back();

		for (int i = F.size()-1; i != -1; i--)
		{
			if (F[i].is_subset_of(test))
				relatively_redundant.push_back(i);
			else
				relatively_essential.push_back(F[i]);

			if (i != 0)
				test[i-1] = F[i];
		}

		for (int i = 0; i < (int)relatively_redundant.size(); i++)
			if (!F[relatively_redundant[i]].is_subset_of(relatively_essential))
				relatively_essential.push_back(F[relatively_redundant[i]]);

		F = relatively_essential;
	}
}

bool mergible(const cover &c1, const cover &c2)
{
	for (int i = 0; i < (int)c1.cubes.size(); i++)
		for (int j = 0; j < (int)c2.cubes.size(); j++)
			if (mergible(c1.cubes[i], c2.cubes[j]))
				return true;

	return false;
}

ostream &operator<<(ostream &os, cover m)
{
	for (int i = 0; i < m.size(); i++)
		os << "[" << m[i] << "] ";

	return os;
}

string to_string(const cover &c, const vector<string> &v, bool safe)
{
	if (c.is_tautology())
		return "1";

	string result;
	for (int i = 0; i < (int)c.cubes.size(); i++)
	{
		string temp = to_string(c.cubes[i], v, safe);

		if (result != "" && temp != "0")
			result += " | ";

		if (temp != "0")
			result += temp;
	}
	return result;
}

cover transition(const cover &s1, const cube &s2)
{
	cover result;
	result.reserve(s1.size());
	for (int i = 0; i < s1.size(); i++)
		result.push_back(transition(s1.cubes[i], s2));

	return result;
}

cover operator~(cover s1)
{
	// Check for empty function
	if (s1.is_null())
		return cover(1);

	// Check for universal cube
	for (int i = 0; i < s1.size(); i++)
		if (s1[i].is_tautology())
			return cover();

	// DeMorgans if only one cube
	if (s1.size() == 1)
		return ~s1[0];

	vector<pair<int, int> > binate_rank;
	for (int i = 0; i < s1.size(); i++)
	{
		if ((int)binate_rank.size() < s1[i].size()*16)
			binate_rank.resize(s1[i].size()*16, pair<int, int>(0, 0));

		for (int j = 0; j < s1[i].size()*16; j++)
		{
			int val = s1[i].get(j);
			if (val == 0)
				binate_rank[j].first++;
			else if (val == 1)
				binate_rank[j].second++;
		}
	}

	for (int i = 0; i < (int)binate_rank.size(); i++)
	{
		// Column of all zeros
		if (binate_rank[i].first == s1.size())
		{
			cover Fc = ~cofactor(s1, i, 0);
			Fc.push_back(cube(i, 1));
			sort(Fc.begin(), Fc.end());
			return Fc;
		}
		else if (binate_rank[i].second == s1.size())
		{
			cover Fc = ~cofactor(s1, i, 1);
			Fc.push_back(cube(i, 0));
			sort(Fc.begin(), Fc.end());
			return Fc;
		}
		else
		{
			binate_rank[i].first += binate_rank[i].second;
			binate_rank[i].second = i;
		}
	}

	cover Fc1, Fc2;
	pair<int, int> uid = *max_element(binate_rank.begin(), binate_rank.end());

	Fc1 = ~cofactor(s1, uid.second, 0);
	Fc2 = ~cofactor(s1, uid.second, 1);

	sort(Fc1.begin(), Fc1.end());
	sort(Fc2.begin(), Fc2.end());

	if ((Fc1.size() + Fc2.size())*s1.size() <= Fc1.size()*Fc2.size())
		return merge_complement_a2(uid.second, Fc1, Fc2, s1);
	else
		return merge_complement_a1(uid.second, Fc1, Fc2, s1);
}

cover merge_complement_a1(int uid, const cover &s0, const cover &s1, const cover &F)
{
	cover result;
	result.reserve(s0.size() + s1.size());

	int i = 0, j = 0;
	while (i < s0.size() || j < s1.size())
	{
		if (j >= s1.size() || (i < s0.size() && s0[i] < s1[j]))
		{
			bool subset_found = are_mutex(s0[i], F);
			for (int k = j; k < s1.size() && !subset_found; k++)
				subset_found = s0[i].is_subset_of(s1[k]);

			result.push_back(s0[i]);

			if (!subset_found)
				result.back().set(uid, 0);
			i++;
		}
		else if (i >= s0.size() || (j < s1.size() && s1[j] < s0[i]))
		{
			bool subset_found = are_mutex(s1[j], F);
			for (int k = i; k < s0.size() && !subset_found; k++)
				subset_found = s1[j].is_subset_of(s0[k]);

			result.push_back(s1[j]);

			if (!subset_found)
				result.back().set(uid, 1);
			j++;
		}
		else
		{
			result.push_back(s0[i]);
			i++;
			j++;
		}
	}

	return result;
}

cover merge_complement_a2(int uid, const cover &s0, const cover &s1, const cover &F)
{
	cover result;
	result.reserve(s0.size() + s1.size());

	int i = 0, j = 0;
	while (i < s0.size() || j < s1.size())
	{
		if (j >= s1.size() || (i < s0.size() && s0[i] < s1[j]))
		{
			cube restriction;
			bool set = false;
			for (int k = 0; k < F.size(); k++)
				if (::distance(s0[i], F[k]) == 1)
				{
					if (set)
						restriction.supercube(F[k]);
					else
					{
						restriction = F[k];
						set = true;
					}
				}

			if (set)
			{
				cube toadd = s0[i];
				if (toadd.size() > restriction.size())
					restriction.extendX(toadd.size() - restriction.size());

				for (int k = 0; k < restriction.size(); k++)
					restriction.values[k] = ~restriction.values[k];

				toadd.supercube(restriction);
				toadd.set(uid, 0);

				result.push_back(toadd);
			}
			else
				return cover(cube());

			i++;
		}
		else if (i >= s0.size() || (j < s1.size() && s1[j] < s0[i]))
		{
			cube restriction;
			bool set = false;
			for (int k = 0; k < F.size(); k++)
				if (::distance(s1[j], F[k]) == 1)
				{
					if (set)
						restriction.supercube(F[k]);
					else
					{
						restriction = F[k];
						set = true;
					}
				}

			if (set)
			{
				cube toadd = s1[j];
				if (toadd.size() > restriction.size())
					restriction.extendX(toadd.size() - restriction.size());

				for (int k = 0; k < restriction.size(); k++)
					restriction.values[k] = ~restriction.values[k];

				toadd.supercube(restriction);
				toadd.set(uid, 1);
				result.push_back(toadd);
			}
			else
				return cover(cube());

			j++;
		}
		else
		{
			result.push_back(s0[i]);
			i++;
			j++;
		}
	}

	return result;
}

cover operator&(cover s1, cover s2)
{
	cover result;
	result.reserve(s1.size()*s2.size());
	for (int i = 0; i < s1.size(); i++)
		for (int j = 0; j < s2.size(); j++)
		{
			result.push_back(cube());
			result.back().values.reserve(max(s1[i].size(), s2[j].size()));

			bool valid = true;
			int m0 = min(s1[i].size(), s2[j].size());
			for (int k = 0; k < m0 && valid; k++)
			{
				result.back().values.push_back(s1[i].values[k] & s2[j].values[k]);
				valid = (((result.back().values.back() | (result.back().values.back() >> 1)) | 0xAAAAAAAA) == 0xFFFFFFFF);
			}

			if (valid)
			{
				for (int k = m0; k < s1[i].size(); k++)
					result.back().values.push_back(s1[i].values[k]);
				for (int k = m0; k < s2[j].size(); k++)
					result.back().values.push_back(s2[j].values[k]);
			}
			else
				result.pop_back();

			if (result.size() >= 128)
				result.minimize();
		}

	result.minimize();

	return result;
}

cover operator&(cover s1, cube s2)
{
	for (int i = 0; i < s1.size(); i++)
		s1[i] &= s2;
	return s1;
}

cover operator&(cube s1, cover s2)
{
	for (int i = 0; i < s2.size(); i++)
		s2[i] &= s1;
	return s2;
}

cover operator&(cover s1, int s2)
{
	if (s2 == 1)
		return s1;
	else
		return cover();
}

cover operator&(int s1, cover s2)
{
	if (s1 == 1)
		return s2;
	else
		return cover();
}

bool are_mutex(const cover &s1, const cube &s2)
{
	for (int i = 0; i < s1.size(); i++)
		if (!are_mutex(s1[i], s2))
			return false;

	return true;
}


bool are_mutex(const cover &s1, const cover &s2)
{
	for (int i = 0; i < s1.size(); i++)
		for (int j = 0; j < s2.size(); j++)
			if (!are_mutex(s1[i], s2[j]))
				return false;

	return true;
}

bool are_mutex(const cover &s1, const cover &s2, const cover &s3)
{
	for (int i = 0; i < s1.size(); i++)
		for (int j = 0; j < s2.size(); j++)
			for (int k = 0; k < s3.size(); k++)
				if (!are_mutex(s1[i], s2[j], s3[k]))
					return false;

	return true;
}

bool are_mutex(const cover &s1, const cover &s2, const cover &s3, const cover &s4)
{
	for (int i = 0; i < s1.size(); i++)
		for (int j = 0; j < s2.size(); j++)
			for (int k = 0; k < s3.size(); k++)
				for (int l = 0; l < s4.size(); l++)
					if (!are_mutex(s1[i], s2[j], s3[k], s4[l]))
						return false;

	return true;
}

cover operator|(cover s1, cover s2)
{
	s1.insert(s1.end(), s2.begin(), s2.end());
	s1.minimize();
	return s1;
}

cover operator|(cover s1, cube s2)
{
	s1.push_back(s2);
	return s1;
}

cover operator|(cube s1, cover s2)
{
	s2.push_back(s1);
	return s2;
}

cover operator|(cover s1, int s2)
{
	if (s2 == 0)
		return s1;
	else
		return cover(1);
}

cover operator|(int s1, cover s2)
{
	if (s1 == 0)
		return s2;
	else
		return cover(1);
}

cover cofactor(const cover &s1, int uid, int val)
{
	cover result;
	result.reserve(s1.size());
	for (int i = 0; i < s1.size(); i++)
	{
		int cmp = s1[i].get(uid);
		if (cmp != 1-val)
		{
			result.push_back(s1[i]);
			result.back().set(uid, 2);
		}
	}

	return result;
}

cover cofactor(const cover &s1, const cube &s2)
{
	cover result;
	result.reserve(s1.size());

	for (int i = 0; i < s1.size(); i++)
	{
		cube entry;
		entry.values.reserve(s1[i].size());

		bool valid = true;
		int j = 0;
		for (j = 0; j < s1[i].size() && j < s2.size() && valid; j++)
		{
			uint32_t b = s2.values[j] & s1[i].values[j];
			b = ~(b | (b >> 1)) & 0x55555555;

			if (b != 0)
				valid = false;
			else
			{
				uint32_t a = (s2.values[j] ^ (s2.values[j] >> 1)) & 0x55555555;
				a = a | (a << 1);
				entry.values.push_back(s1[i].values[j] | a);
			}
		}

		if (valid)
		{
			for (; j < s1[i].size(); j++)
				entry.values.push_back(s1[i].values[j]);

			result.push_back(entry);
		}
	}

	return result;
}

cube supercube_of_complement(const cover &s)
{
	// Check for empty function
	if (s.size() == 0)
		return cube();

	// Check for universal cube
	for (int i = 0; i < s.size(); i++)
		if (s[i].is_tautology())
			return cube(0);

	// only one cube
	if (s.size() == 1)
		return supercube_of_complement(s[0]);

	vector<pair<int, int> > binate_rank;
	for (int i = 0; i < s.size(); i++)
	{
		if ((int)binate_rank.size() < s[i].size()*16)
			binate_rank.resize(s[i].size()*16, pair<int, int>(0, 0));

		for (int j = 0; j < s[i].size()*16; j++)
		{
			int val = s[i].get(j);
			if (val == 0)
				binate_rank[j].first++;
			else if (val == 1)
				binate_rank[j].second++;
		}
	}

	vector<pair<int, int> > columns;
	for (int i = 0; i < (int)binate_rank.size(); i++)
	{
		// Column of all zeros
		if (binate_rank[i].first == s.size())
			columns.push_back(pair<int, int>(i, 0));
		else if (binate_rank[i].second == s.size())
			columns.push_back(pair<int, int>(i, 1));

		binate_rank[i].first += binate_rank[i].second;
		binate_rank[i].second = i;
	}

	if (columns.size() > 1)
		return cube();

	pair<int, int> uid = *max_element(binate_rank.begin(), binate_rank.end());

	if (uid.first > 0)
	{
		cube Fc1 = supercube_of_complement(cofactor(s, uid.second, 0));
		cube Fc2 = supercube_of_complement(cofactor(s, uid.second, 1));

		Fc1.sv_intersect(uid.second, 0);
		Fc2.sv_intersect(uid.second, 1);

		return supercube(Fc1, Fc2);
	}

	cube result;
	for (int i = 0; i < s.size(); i++)
		result.intersect(supercube_of_complement(s[i]));

	return result;
}

bool operator==(const cover &s1, const cover &s2)
{
	return (are_mutex(s1, ~s2) && are_mutex(~s1, s2));
}

bool operator==(const cover &s1, const cube &s2)
{
	return (are_mutex(s1, ~s2) && are_mutex(~s1, s2));
}

bool operator==(const cube &s1, const cover &s2)
{
	return (are_mutex(s1, ~s2) && are_mutex(~s1, s2));
}

bool operator==(cover s1, int s2)
{
	return ((s2 == 0 && s1.is_null()) || (s2 == 1 && s1.is_tautology()));
}

bool operator==(int s1, cover s2)
{
	return ((s1 == 0 && s2.is_null()) || (s1 == 1 && s2.is_tautology()));
}

bool operator!=(const cover &s1, const cover &s2)
{
	return (!are_mutex(s1, ~s2) || !are_mutex(~s1, s2));
}

bool operator!=(const cover &s1, const cube &s2)
{
	return (!are_mutex(s1, ~s2) || !are_mutex(~s1, s2));
}

bool operator!=(const cube &s1, const cover &s2)
{
	return (!are_mutex(s1, ~s2) || !are_mutex(~s1, s2));
}

bool operator!=(cover s1, int s2)
{
	return (!(s2 == 0 && s1.is_null()) && !(s2 == 1 && s1.is_tautology()));
}

bool operator!=(int s1, cover s2)
{
	return (!(s1 == 0 && s2.is_null()) && !(s1 == 1 && s2.is_tautology()));
}