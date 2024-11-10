#include <iostream>
#include "StrukturyBazDanych.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world!\n";
	StrukturyBazDanych::generateRandomRecords("random10000.bin", 1000000);
	//StrukturyBazDanych::printOutFile("random10000.bin");
	
	StrukturyBazDanych::sortUsingLargeBuffers("random10000.bin", 1001, 240, true);
	
	return 0;
}