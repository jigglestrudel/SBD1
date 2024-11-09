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

void generateOrderedRecords(const char* filePath, int n)
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
			record.a = (i + 1);
			record.b = (i + 1);
			record.h = (i + 1);

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
			try 
			{
				fileReader.readRecord(&record);
				std::cout << record << '\n';
			}
			catch (FileManagerNoRecords& e)
			{
				break;
			}
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

void StrukturyBazDanych::sortUsingLargeBuffers(const char* filePath, int buffer_count, size_t block_size, bool print_debug)
{
	int b = block_size / sizeof(Record);
	if (print_debug)
	{
		std::cout << "Sorting file " << filePath << " using n=" << buffer_count << " buffers\n"
			<< "of size " << block_size << " bytes\n"
			<< "giving blocking factor b=" << b << "\n";
	}

	//	Stage 0. (Creating the BIG BUFFERS)

	// they will take up a big connected chunk of memory
	std::byte* buffer_space = new std::byte[block_size * buffer_count];
	Buffer** buffers = new Buffer*[buffer_count];
	for (int i = 0; i < buffer_count; i++)
	{
		buffers[i] = new Buffer(buffer_space + i * block_size, block_size);
	}

	//  Stage 1. (Creating runs)
	//	1. read n*b records and sort them
	
	//	here we want the FileReaders' buffer to take up all big buffers
	//	for them to share the buffer between reading and writing
	long int nb = buffer_count * b;
	//char* fileName = new char[14];
	FileManager bigRunReader(filePath, block_size);
	bigRunReader.startReading();
	int fileCounter = 0;

	while (!bigRunReader.wasEndOfRecordsReached())
	{
		long int records_read = 0;
		
			
		for (records_read = 0; records_read < nb; records_read++)
		{
			try
			{
				bigRunReader.readRecord((Record*)(buffer_space + (sizeof(Record) * records_read)));
			}
			catch (FileManagerNoRecords& e)
			{
				break;
			}
		}

		if (bigRunReader.wasEndOfRecordsReached())
			break;
		
		//std::qsort(buffer_space, records_read, sizeof(Record), compareRecords);

		//	2. write the big run onto disk
		fileCounter++;
		std::string fileName = std::to_string(fileCounter) + ".bin";

		FileManager bigRunWriter(fileName.c_str(), block_size);
		bigRunWriter.startWriting();

		for (long int i = 0; i < records_read; i++)
		{
			bigRunWriter.writeRecord((Record*)(buffer_space + (sizeof(Record) * i)));
		}

		bigRunWriter.finishAndStop();

		if (print_debug)
			printOutFile(fileName.c_str());

		//	3. repeat until end of records
	}
	bigRunReader.finishAndStop();

	//	Stage 2. (Merging)
	//	1. load n-1 runs
	//	2. merge them using the n-th buffer
	//	3. repeat until one run

	std::cout << "Done!\n";
	delete[] buffers;
	delete[] buffer_space;
}
