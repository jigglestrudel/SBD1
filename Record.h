#pragma once
#include <iostream>
#include <iomanip>

class Record
{
public:
	double a;
	double b;
	double h;

	double area() const;
	friend bool operator<(const Record& l, const Record& r);
	friend inline bool operator> (const Record& lhs, const Record& rhs);
	friend inline bool operator<=(const Record& lhs, const Record& rhs);
	friend inline bool operator>=(const Record& lhs, const Record& rhs);
	friend std::ostream& operator<<(std::ostream& os, const Record& obj);
};

int compareRecords(const void* left, const void* right);