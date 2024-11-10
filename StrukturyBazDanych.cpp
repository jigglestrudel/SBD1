#include "StrukturyBazDanych.h"

void StrukturyBazDanych::generateRandomRecords(const char* filePath, int n)
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

void StrukturyBazDanych::generateOrderedRecords(const char* filePath, int n)
{
	std::cout << "Writing " << n << " ordered records to " << filePath << "\n";

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

void StrukturyBazDanych::generateRecordsFromUser(const char* filePath, int n)
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

void StrukturyBazDanych::printOutFile(const char* filePath)
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
		std::cout << "===============================\n";
		fileReader.finishAndStop();
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void StrukturyBazDanych::copyFileToFileByRecord(const char* src, const char* dest)
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
	long long unsigned int disc_writes = 0;
	long long unsigned int disc_reads = 0;
	long long unsigned int cycle_count = 0;

	int b = block_size / sizeof(Record);

	std::byte* buffer_space = new std::byte[block_size * buffer_count];
	Buffer** buffers = new Buffer * [buffer_count];

	long int nb = buffer_count * b;
	FileManager big_run_reader(filePath, block_size);

	unsigned long long int file_count = 0;
	std::queue<std::string> run_files_names;
	std::queue<std::string> new_run_files_names;

	std::vector<FileManager*> run_readers;
	int runs_opened = 0;
	
	long long int num = 0x7FF0000000000000;
	double inf = *(double*)&num;
	const Record infRecord(inf, inf, inf);

	
	if (print_debug)
	{
		std::cout << "Sorting file " << filePath << " using n=" << buffer_count << " buffers\n"
			<< "of size " << block_size << " bytes\n"
			<< "giving blocking factor b=" << b << "\n";
	}

	//	Stage 0. (Creating the BIG BUFFERS)

	// they will take up a big connected chunk of memory
	
	for (int i = 0; i < buffer_count; i++)
	{
		buffers[i] = new Buffer(buffer_space + i * block_size, block_size);
	}

	std::cout << "Stage one: Creating runs\n";
	std::cout << "Do you want to see the created runs? [y/n]>";
	char user_input;
	bool print_runs = false;
	std::cin >> user_input;
	if (user_input == 'y' or user_input == 'Y')
		print_runs = true;

	//  Stage 1. (Creating runs)
	//	1. read n*b records and sort them

	//	here we want the FileReaders' buffer to take up all big buffers
	//	for them to share the buffer between reading and writing
	big_run_reader.startReading();

	while (!big_run_reader.wasEndOfRecordsReached())
	{
		long int records_read = 0;

		for (records_read = 0; records_read < nb; records_read++)
		{
			try
			{
				big_run_reader.readRecord((Record*)(buffer_space + (sizeof(Record) * records_read)));
			}
			catch (FileManagerNoRecords& e)
			{
				break;
			}
		}

		if (big_run_reader.wasEndOfRecordsReached())
			break;

		std::qsort(buffer_space, records_read, sizeof(Record), compareRecords);

		//	2. write the big run onto disk
		file_count++;
		std::string file_name = std::to_string(file_count) + ".bin";
		run_files_names.push(file_name);

		FileManager big_run_writer(file_name.c_str(), block_size);
		big_run_writer.startWriting();

		for (long int i = 0; i < records_read; i++)
		{
			big_run_writer.writeRecord((Record*)(buffer_space + (sizeof(Record) * i)));
		}

		
		big_run_writer.finishAndStop();
		disc_writes += big_run_writer.getDiscWriteCount();

		if (print_runs)
			printOutFile(file_name.c_str());

		//	3. repeat until end of records
	}
	big_run_reader.finishAndStop();
	disc_reads += big_run_reader.getDiscReadCount();
	
	
	std::cout << "Stage one finished!\n";
	
	//	Stage 2. (Merging)
	//	1. load n-1 runs

	std::cout << "Stage two: Merging\n";
	std::cout << "Do you want to see the created runs? [y/n]>";
	print_runs = false;
	std::cin >> user_input;
	if (user_input == 'y' or user_input == 'Y')
		print_runs = true;
	
	do {
		cycle_count++;
		std::cout << "Cycle " << cycle_count << "\n";

		while (run_files_names.size() > 0)
		{
			std::cout << "There are " << run_files_names.size() << " runs\n";

			run_readers.clear();
			std::cout << "Mering runs: ";
			for (runs_opened = 0; runs_opened < buffer_count - 1 && !run_files_names.empty(); runs_opened++)
			{
				std::cout << run_files_names.front() << " ";
				run_readers.push_back(new FileManager(run_files_names.front().c_str(), block_size, buffers[runs_opened]));
				run_readers.back()->startReading();
				run_files_names.pop();
			}
			std::cout << "\n";

			//	2. merge them using the n-th buffer
			file_count++;
			std::string file_name = std::to_string(file_count) + ".bin";
			new_run_files_names.push(file_name);
			FileManager run_merger(file_name.c_str(), block_size, buffers[buffer_count - 1]);
			run_merger.startWriting();

			//merging
			TournamentTree tournament_tree(runs_opened);
			for (int i = 0; i < runs_opened; i++)
			{
				Record record;
				run_readers[i]->readRecord(&record);
				tournament_tree.addPlayer(record);
			}
			tournament_tree.runTournament();

			do
			{
				std::pair<Record, int> winner = tournament_tree.getWinner();
				run_merger.writeRecord(&(winner.first));
				Record new_player;
				try
				{
					run_readers[winner.second]->readRecord(&new_player);
				}
				catch (FileManagerNoRecords& e)
				{
					new_player = INFRECORD;
				}
				tournament_tree.changePlayer(winner.second, new_player);
				tournament_tree.runTournamentForPlayer(winner.second);
				
			} while (!(tournament_tree.getWinner().first == INFRECORD));
			
			run_merger.finishAndStop();
			disc_writes += run_merger.getDiscWriteCount();

			if (print_runs)
			{
				printOutFile(file_name.c_str());
			}

			for (FileManager* fm : run_readers)
			{
				
				fm->finishAndStop();
				disc_reads += fm->getDiscReadCount();

				delete fm;
			}
			
		}
		for (; !new_run_files_names.empty(); new_run_files_names.pop())
			run_files_names.push(new_run_files_names.front());
		std::cout << "Cycle " << cycle_count << " done\n";
	} while (run_files_names.size() != 1);


	//	3. repeat until one run



	std::cout << "Done!\n";

	
	delete[] buffers;
	delete[] buffer_space;

	std::cout << "Do you want to see the output run? [y/n]>";
	std::cin >> user_input;
	if (user_input == 'y' or user_input == 'Y')
		printOutFile(run_files_names.front().c_str());
	


	std::cout << "Cycles needed: " << cycle_count << "\n";
	std::cout << "Disk Reads: " << disc_reads << "\n";
	std::cout << "Disk Writes: " << disc_writes << "\n";
	std::cout << "Total disk cost:" << disc_reads + disc_writes << "\n";
}
