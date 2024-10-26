#include "StrukturyBazDanych.h"

void generateRandomRecords(const char* filePath, int n)
{
	srand(time(NULL));
	std::cout << "Writing " << n << " random records to " << filePath << "\n";

	try
	{
		FileManager fileWriter(filePath, TWO_KILOBYTES);
		fileWriter.startWriting();

		for (int i = 0; i < n; i++)
		{
			Record record;
			record.a = 100.f * (rand() % rand_max) / rand_max;
			record.b = 100.f * (rand() % rand_max) / rand_max;
			record.h = 100.f * (rand() % rand_max) / rand_max;

			fileWriter.writeRecord(&record);
		}

		fileWriter.finishAndStop();
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void generateRecordsFromUser(const char* filePath, int n)
{
	std::cout << "Writing " << n << " user-generated records to " << filePath << "\n";
	try
	{
		FileManager fileWriter(filePath, TWO_KILOBYTES);
		fileWriter.startWriting();

		for (int i = 0; i < n; i++)
		{
			Record record;
			std::cout << "record no. " << i + 1 << '\n';
			std::cout << "a=";
			std::cin >> record.a;
			std::cout << "b=";
			std::cin >> record.b; 
			std::cout << "h=";
			std::cin >> record.h;

			fileWriter.writeRecord(&record);
		}

		fileWriter.finishAndStop();
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void printOutFile(const char* filePath)
{
	std::cout << "Printing out records from file: " << filePath << '\n';
	std::cout << "a\tb\th\tarea\n";
	try
	{
		FileManager fileReader(filePath, TWO_KILOBYTES);
		fileReader.startReading();
		
		while (!fileReader.wasEndOfRecordsReached())
		{
			Record record;
			fileReader.readRecord(&record);
			std::cout << record << '\n';
		}

		fileReader.finishAndStop();
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void copyFileToFileByRecord(const char* src, const char* dest)
{
	std::cout << "Copying records: " << src  << " >> " << dest << '\n';
	try
	{
		FileManager fileReader(src, TWO_KILOBYTES);
		FileManager fileWriter(dest, TWO_KILOBYTES);
		fileReader.startReading();
		fileWriter.startWriting();

		while (!fileReader.wasEndOfRecordsReached())
		{
			Record record;
			fileReader.readRecord(&record);
			fileWriter.writeRecord(&record);
		}

		fileReader.finishAndStop();
		fileWriter.finishAndStop();
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
