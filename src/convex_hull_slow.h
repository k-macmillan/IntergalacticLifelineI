#ifndef CONVEX_HULL_SLOW_H
#define CONVEX_HULL_SLOW_H

#include <iostream>
#include <vector>
namespace convhull
{

	// Not even remotely efficient, that's ok
    struct {
        bool operator()(sc2::Unit a, sc2::Unit b) const
        {   
            return atan2(a.pos.y,a.pos.x) < atan2(b.pos.y,b.pos.x);	// assumes origin start 0,0
        }   
    } customLess;

	void OrderPoints(std::vector<sc2::Unit> &units)
	{
		sort(units.begin(), units.end(), customLess);
	}


	bool OnSegment(const sc2::Unit &p, const sc2::Unit &q, const sc2::Unit &r)
	{
		return q.pos.x <= std::max(p.pos.x, r.pos.x) && q.pos.x >= std::min(p.pos.x, r.pos.x) &&
			   q.pos.y <= std::max(p.pos.y, r.pos.y) && q.pos.y >= std::min(p.pos.y, r.pos.y);
	}

	int Orientation(const sc2::Unit &p, const sc2::Unit &q, const sc2::Unit &r)
	{
		int ret_val = (q.pos.y - p.pos.y) * (r.pos.x - q.pos.x) - (q.pos.x - p.pos.x) * (r.pos.y - q.pos.y);
		if (ret_val == 0)
			return 0;
		
		return ret_val > 0 ? 1 : 2;
	}

	std::vector<sc2::Unit> ConvexHull(sc2::Units units)
	{	
		unsigned int size = units.size();
		if (size < 3)
			return {};

		std::vector<sc2::Unit> ret_val;
		ret_val.reserve(size);

		unsigned int left_most = 0;
		for (unsigned int i = 0; i < size; ++i)
			if (units[i]->pos.x < units[left_most]->pos.x)
				left_most = i;

		int p = left_most, q;

		do
		{
			ret_val.push_back(*units[p]);
			q = (p + 1) % size;
			for (unsigned int i = 0; i < size; ++i)
				if (Orientation(*units[p], *units[i], *units[q]) == 2)
					q = i;

			p = q;
		}while (p != left_most);

		OrderPoints(ret_val);
		return ret_val;
	}

	float Area(const std::vector<sc2::Unit> &units)
	{
		unsigned int n = units.size();
		float ret_val = 0.0f;

		for (unsigned int i = 0, j = n - 1; i < n; ++i)
		{
			ret_val += (units[j].pos.x + units[i].pos.x) * (units[j].pos.y - units[i].pos.y);
			j = i;
		}
		return std::fabs(ret_val / 2.0f);
	}

	float Distance(const sc2::Unit &u1, const sc2::Unit &u2)
	{
		return std::sqrt((u2.pos.x - u1.pos.x) * (u2.pos.x - u1.pos.x) + (u2.pos.y - u1.pos.y) * (u2.pos.y - u1.pos.y));
	}

	float PerimeterLength(const std::vector<sc2::Unit> &units)
	{		
		int size = units.size() - 2;	// since we go to i + 1 we have to be 2 less than size
		if (size < 2)
			return 0.0f;

		float ret_val = 0.0f;
		for (int i = 0; i < size; ++i)
			ret_val += Distance(units[i], units[i + 1]);
		ret_val += Distance(units[size + 1], units[0]);
		return ret_val;
	}
}

#endif