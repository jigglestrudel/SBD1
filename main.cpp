#include <iostream>
#include <chrono>
#include "StrukturyBazDanych.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world!\n";

	/*std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	StrukturyBazDanych::generateRandomRecords("10milionT.bin", 10000000);
	std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds.\n";*/
	//StrukturyBazDanych::printOutFile("random10000.bin");

	//StrukturyBazDanych::generateRandomRecords("random.bin", 100);
	StrukturyBazDanych sbd;
	sbd.runTUI();
	//StrukturyBazDanych::generateRandomRecords("random.bin", 100000);
	
	//StrukturyBazDanych::sortUsingLargeBuffers("random.bin", 10, 240, true);
	
	return 0;
}