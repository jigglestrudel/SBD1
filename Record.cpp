#include "Record.h"

double Record::area() const
{
	return (a + b) * 0.5 * h;
}

bool operator<(const Record& l, const Record& r)
{
	return l.area() < r.area();
}

inline bool operator> (const Record& lhs, const Record& rhs) { return rhs < lhs; }
inline bool operator<=(const Record& lhs, const Record& rhs) { return !(lhs > rhs); }
inline bool operator>=(const Record& lhs, const Record& rhs) { return !(lhs < rhs); }

std::ostream& operator<<(std::ostream& os, const Record& obj)
{
	os.precision(3);
	os  << std::fixed << obj.a << '\t'
		<< obj.b << '\t'
		<< obj.h << '\t'
		<< obj.area();
	return os;
}
