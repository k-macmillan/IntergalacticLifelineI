#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <vector>
#include <stack>
#include <algorithm>
#include "sc2api/sc2_unit.h"

// Graham's Scan SC2
class ConvexHull
{
public:
	ConvexHull(std::vector<sc2::Unit> units){
		if (units.size() < 3)
			return;

		vec_ptr = &units;
		conv_hull_.reserve(units.size());
		FindBottomLeftUnit(units);
		current_ = units[bottom_most_index_];
		PerformConvHull();
	}

	std::vector<sc2::Unit> GetConvexHull()
	{
		return conv_hull_;
	}

	float GetArea()
	{
		float ret_val = 0.0f;

		return ret_val;
	}

private:
	unsigned int bottom_most_index_ = 0;
	sc2::Unit current_;

	std::vector<sc2::Unit> conv_hull_;
	std::vector<sc2::Unit> *vec_ptr;


	void FindBottomLeftUnit(const std::vector<sc2::Unit> &units)
	{
		for (unsigned int i = 0; i < units.size(); ++i)
		{
			if (units[i].pos.y < units[bottom_most_index_].pos.y)
			{
				bottom_most_index_ = i;
			}
			else if (units[i].pos.y == units[bottom_most_index_].pos.y)
			{
				if (units[i].pos.x < units[bottom_most_index_].pos.x)
					bottom_most_index_ = i;
			}

		}
	}

	sc2::Unit NextTop(std::stack<sc2::Unit> &stk)
	{
		sc2::Unit u = stk.top();
		stk.pop();
		sc2::Unit ret_val = stk.top();
		stk.push(u);
		return ret_val;
	}

	void SwapFunc(const unsigned int &u1, const unsigned int &u2)
	{
		// std::iter_swap((*vec_ptr).begin() + u1, (*vec_ptr).begin() + u2);
		std::swap(vec_ptr[u1], vec_ptr[u2]);	// Since vec_ptr is a pointer this works
	}

	float DistSquared(const sc2::Unit &u1, const sc2::Unit &u2)
	{
		return (u1.pos.x - u2.pos.x)*(u1.pos.x - u2.pos.x) + (u1.pos.y - u2.pos.y)*(u1.pos.y - u2.pos.y);
	}

	int Orientation(const sc2::Unit &p, const sc2::Unit &q, const sc2::Unit &r)
	{
		int ret_val = (q.pos.y - p.pos.y) * (r.pos.x - q.pos.x) - (q.pos.x - p.pos.x) * (r.pos.y - q.pos.y);
		if (ret_val == 0)
			return 0;
		
		return ret_val > 0 ? 1 : 2;
	}
	
	int compare(const void *void_pointer_1, const void *void_pointer_2)
	{
		sc2::Unit *u1 = (sc2::Unit *)void_pointer_1;
		sc2::Unit *u2 = (sc2::Unit *)void_pointer_2;

		int orientation = Orientation(current_, *u1, *u2);

		if (orientation == 0)
			return (DistSquared(current_, *u2) >= DistSquared(current_, *u1)) ? -1 : 1;
		
		return orientation == 2 ? -1 : 1;
	}

	void PerformConvHull()
	{
		SwapFunc(0, bottom_most_index_);
		current_ = (*vec_ptr)[0];

		// Does not work due to static member function issues. Need to address later
		// std::qsort(&(*vec_ptr)[1], (*vec_ptr).size() - 1, sizeof(sc2::Unit), compare);
	}

};



#endif