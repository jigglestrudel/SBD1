#pragma once
#include <iostream>
#include <iomanip>
#include <limits>

class Record
{
public:
	double a;
	double b;
	double h;

	Record() : a(0.), b(0.), h(0.) {};
	Record(double a, double b, double h) : a(a), b(b), h(h) {};

	double area() const;
	void print();

};

bool operator<(const Record& l, const Record& r);
bool operator==(const Record& l, const Record& r);
inline bool operator!=(const Record& l, const Record& r) { return !(l == r); }
inline bool operator> (const Record& lhs, const Record& rhs) { return rhs < lhs; }
inline bool operator<=(const Record& lhs, const Record& rhs) { return !(lhs > rhs); }
inline bool operator>=(const Record& lhs, const Record& rhs) { return !(lhs < rhs); }
std::ostream& operator<<(std::ostream& os, const Record& obj);


int compareRecords(const void* left, const void* right);

const Record INFRECORD = Record(std::numeric_limits<double>::infinity(),
								std::numeric_limits<double>::infinity(),
								std::numeric_limits<double>::infinity());

