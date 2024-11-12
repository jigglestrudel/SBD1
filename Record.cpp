#include "Record.h"

double Record::area() const
{
	return (a + b) * 0.5 * h;
}

void Record::print()
{
	std::printf("%10.2f%10.2f%10.2f%10.2f", a, b, h, area());
}

bool operator<(const Record& l, const Record& r)
{
	return l.area() < r.area();
}



bool operator==(const Record& l, const Record& r)
{
	//// comparing byte form not value
	//bool a = *(long long int*) & l.a == *(long long int*) & r.a;
	//bool b = *(long long int*) & l.b == *(long long int*) & r.b;
	//bool h = *(long long int*) & l.h == *(long long int*) & r.h;
	//return  a && b && h;
	return l.a == r.a && l.b == r.b && l.h == r.h;
}


std::ostream& operator<<(std::ostream& os, const Record& obj)
{
	os.precision(3);
	os  << std::fixed << obj.a << '\t'
		<< obj.b << '\t'
		<< obj.h << '\t'
		<< obj.area();
	
	return os;
}


int compareRecords(const void* left, const void* right)
{
	if ((*(Record*)left) > (*(Record*)right))
		return 1;
	if ((*(Record*)left) < (*(Record*)right))
		return -1;
	return 0;
}


