#include <iostream>
#include "StrukturyBazDanych.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world!\n";
	generateRandomRecords("random500.bin", 500);
	printOutFile("random500.bin");
	
	StrukturyBazDanych::sortUsingLargeBuffers("random500.bin", 10, 500, true);
	
	return 0;
}