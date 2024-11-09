#include <iostream>
#include "StrukturyBazDanych.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world!\n";
	generateOrderedRecords("random100.bin", 100);
	printOutFile("random100.bin");
	
	StrukturyBazDanych::sortUsingLargeBuffers("random100.bin", 5, 48, true);
	
	return 0;
}