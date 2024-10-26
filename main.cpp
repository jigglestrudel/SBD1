#include <iostream>
#include "StrukturyBazDanych.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world!\n";
	generateRandomRecords("random500.bin", 500);
	printOutFile("random500.bin");

	generateRecordsFromUser("user5.bin", 5);
	copyFileToFileByRecord("user5.bin", "copyuser5.bin");
	printOutFile("copyuser5.bin");
	return 0;
}