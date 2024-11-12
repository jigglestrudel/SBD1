#include "StrukturyBazDanych.h"

StrukturyBazDanych::StrukturyBazDanych()
{
}

StrukturyBazDanych::~StrukturyBazDanych()
{
}

void option_1()
{
	std::string file_name;
	int n;
	std::cout << "File name>";
	std::cin >> file_name;
	std::cout << "How many records>";
	std::cin >> n;
	StrukturyBazDanych::generateRandomRecords(file_name.c_str(), n);
}

void option_2()
{
	std::string file_name;
	int n;
	std::cout << "File name>";
	std::cin >> file_name;
	std::cout << "How many records>";
	std::cin >> n;
	StrukturyBazDanych::generateRecordsFromUser(file_name.c_str(), n);
}

void option_3()
{
	std::string file_name;
	int n;
	std::cout << "File name>";
	std::cin >> file_name;
	
	StrukturyBazDanych::printOutFile(file_name.c_str());
}

void option_4()
{
	std::string file_name;
	int n;
	std::cout << "File name>";
	std::cin >> file_name;

	StrukturyBazDanych::printRecordNumber(file_name.c_str());
}

void option_5()
{
	std::string file_name;
	int n, b;
	std::cout << "File name>";
	std::cin >> file_name;
	std::cout << "How many buffers (n)>";
	std::cin >> n;
	std::cout << "Blocking factor (b)>";
	std::cin >> b;
	StrukturyBazDanych::sortUsingLargeBuffers(file_name.c_str(), n, b*sizeof(Record), true);
}

void StrukturyBazDanych::runTUI()
{
	bool is_running = true;
	
	while (is_running)
	{
		std::string chosen_option;
		std::cout << "DATABASES STRUCTURES PROJECT 1\nby Tomasz Krepa 193047\n\n";
		std::cout << "Menu:\n";
		std::cout << "1. Generate a random record file\n";
		std::cout << "2. Generate a user inputted record file\n";
		std::cout << "3. Print out records from a file\n";
		std::cout << "4. Count records in a file\n";
		std::cout << "5. Sort a file using large buffers\n";
		std::cout << "6. Exit the program\n>";
		std::cin >> chosen_option;
		switch (chosen_option[0])
		{
		case '1':
			option_1();
			break;
		case '2':
			option_2();
			break;
		case '3':
			option_3();
			break;
		case '4':
			option_4();
			break;
		case '5':
			option_5();
			break;
		case '6':
			is_running = false;
			break;

		default:
			break;
		}
	}

}


void StrukturyBazDanych::generateRandomRecords(const char* filePath, int n)
{
	srand(time(NULL));
	std::cout << "Writing " << n << " random records to " << filePath << "\n";
	std::cout << "____________________" << "\n";

	try
	{
		FileManager fileWriter(filePath, HALF_KILOBYTE);
		fileWriter.startWriting();

		for (int i = 0; i < n; i++)
		{
			Record record;
			record.a = 100.f * (rand() % rand_max) / rand_max;
			record.b = 100.f * (rand() % rand_max) / rand_max;
			record.h = 100.f * (rand() % rand_max) / rand_max;

			fileWriter.writeRecord(&record);

			if (i % (n / 20) == (n / 20)-1)
				std::cout << "#";
		}
		
		fileWriter.finishAndStop();
		std::cout << "\nDone! - "<< fileWriter.getDiscWriteCount() << " disk writes \n";
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
		FileManager fileWriter(filePath, HALF_KILOBYTE);
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
		FileManager fileWriter(filePath, HALF_KILOBYTE);
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
	std::cout << "        a        b        h        area      runs\n";
	try
	{
		FileManager fileReader(filePath, HALF_KILOBYTE);
		fileReader.startReading();
		Record record1, record2;
		record1 = INFRECORD;
		
		while (!fileReader.wasEndOfRecordsReached())
		{
			try 
			{
				fileReader.readRecord(&record2);
				record2.print();
				if (record1 > record2)
				{
					std::cout << "\t<<<\n";
				}
				else
					std::cout << "\n";
				record1 = record2;
			}
			catch (FileManagerNoRecords& e)
			{
				break;
			}
		}
		std::cout << "===============================\n";
		fileReader.finishAndStop();
		std::cout << "Done! - " << fileReader.getDiscReadCount() << " disk reads \n";
	}
	catch (const FileManagerException& e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void StrukturyBazDanych::printRecordNumber(const char* filePath)
{
	std::cout << "File " << filePath;
	try
	{
		long long unsigned int record_num = 0;
		FileManager fileReader(filePath, HALF_KILOBYTE);
		fileReader.startReading();
		Record record1, record2;
		bool is_sorted = true;

		while (!fileReader.wasEndOfRecordsReached())
		{
			try
			{
				fileReader.readRecord(&record2);
				if (record1 > record2)
				{
					is_sorted = false;
				}
				record1 = record2;
				record_num += 1;
			}
			catch (FileManagerNoRecords& e)
			{
				break;
			}
		}
		std::cout << " has " << record_num << (is_sorted ? " sorted" : " unsorted") << " records\n";
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

long long unsigned int StrukturyBazDanych::divideIntoRuns(const char* filePath, size_t block_size, long int nb, std::byte* buffer_space, std::queue<std::string>& run_files_names, std::stack<std::string>& files_to_delete, long long unsigned int* file_count)
{
	bool print_runs = askUser("Do you want to see the created runs?");
	bool print_counts = askUser("Do you want to check the record numbers?");
	long long unsigned int stage_1_disc_writes = 0;
	long long unsigned int stage_1_disc_reads = 0;

	//  Stage 1. (Creating runs)
	//	1. read n*b records and sort them

	//	here we want the FileReaders' buffer to take up all big buffers
	//	for them to share the buffer between reading and writing
	FileManager big_run_reader(filePath, block_size);
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

		if (records_read == 0 && big_run_reader.wasEndOfRecordsReached())
			break;

		std::qsort(buffer_space, records_read, sizeof(Record), compareRecords);

		//	2. write the big run onto disk
		(*file_count)++;
		std::string file_name = std::to_string(*file_count) + ".bin";
		run_files_names.push(file_name);
		files_to_delete.push(file_name);

		FileManager big_run_writer(file_name.c_str(), block_size);
		big_run_writer.startWriting();

		for (long int i = 0; i < records_read; i++)
		{
			big_run_writer.writeRecord((Record*)(buffer_space + (sizeof(Record) * i)));
		}


		big_run_writer.finishAndStop();
		stage_1_disc_writes += big_run_writer.getDiscWriteCount();

		if (print_runs)
			printOutFile(file_name.c_str());
		if (print_counts)
			printRecordNumber(file_name.c_str());

		//	3. repeat until end of records
	}
	big_run_reader.finishAndStop();
	stage_1_disc_reads += big_run_reader.getDiscReadCount();
	
	std::cout << stage_1_disc_reads << " disk reads\n" << stage_1_disc_writes << " disk writes\n";
	return stage_1_disc_reads + stage_1_disc_writes;
}

long long unsigned int StrukturyBazDanych::mergeRunsToFile(std::string file_name, size_t block_size, Buffer** buffers, int buffer_count, std::vector<FileManager*> run_readers, int runs_opened)
{
	FileManager run_merger(file_name.c_str(), block_size, buffers[buffer_count - 1]);
	run_merger.startWriting();

	//merging
	TournamentTree* tournament_tree = new TournamentTree(runs_opened);
	Record record;
	for (int i = 0; i < runs_opened; i++)
	{
		run_readers[i]->readRecord(&record);
		tournament_tree->addPlayer(&record);
	}
	tournament_tree->runTournament();

	do
	{
		int winner = tournament_tree->getWinner();
		run_merger.writeRecord(tournament_tree->getWinnerRecord());
		Record new_player;
		try
		{
			run_readers[winner]->readRecord(&new_player);
		}
		catch (FileManagerNoRecords& e)
		{
			new_player = INFRECORD;
		}
		tournament_tree->changePlayer(winner, &new_player);
		tournament_tree->runTournamentForPlayer(winner);

	} while (!(*tournament_tree->getWinnerRecord() == INFRECORD));

	delete tournament_tree;
	run_merger.finishAndStop();
	return run_merger.getDiscWriteCount();
}

long long unsigned int StrukturyBazDanych::mergeUntilOneFile(long long unsigned int * cycle_count, std::queue<std::string>& run_files_names, std::stack<std::string>& files_to_delete, long long unsigned int *file_count, size_t block_size, Buffer** buffers, int buffer_count)
{
	long long unsigned int stage_2_disk_cost = 0;

	std::queue<std::string> new_run_files_names;

	std::vector<FileManager*> run_readers;
	int runs_opened = 0;

	bool print_runs = askUser("Do you want to see the merged runs?");
	bool print_counts = askUser("Do you want to see their counts?");

	do {
		(*cycle_count)++;
		std::cout << "Cycle " << *cycle_count << "\n";

		while (run_files_names.size() > 0)
		{
			std::cout << "There are " << run_files_names.size() << " runs\n";

			if (run_files_names.size() == 1)
			{
				// if we have only one run left to merge we carry it over to the next cycle
				std::cout << "nothing to merge with\n";
				new_run_files_names.push(run_files_names.front());
				run_files_names.pop();
				continue;
			}

			run_readers.clear();
			std::cout << "Mering runs: ";
			for (runs_opened = 0; runs_opened < buffer_count - 1 && !run_files_names.empty(); runs_opened++)
			{
				std::cout << run_files_names.front() << " ";
				//printRecordNumber(run_files_names.front().c_str());
				run_readers.push_back(new FileManager(run_files_names.front().c_str(), block_size, buffers[runs_opened]));
				run_readers.back()->startReading();
				run_files_names.pop();
			}
			std::cout << "\n";

			//	2. merge them using the n-th buffer
			(*file_count)++;
			std::string file_name = std::to_string(*file_count) + ".bin";
			new_run_files_names.push(file_name);
			files_to_delete.push(file_name);

			stage_2_disk_cost += mergeRunsToFile(file_name, block_size, buffers, buffer_count, run_readers, runs_opened);

			if (print_runs)
			{
				printOutFile(file_name.c_str());
			}
			if (print_counts)
				printRecordNumber(file_name.c_str());

			for (FileManager* fm : run_readers)
			{
				fm->finishAndStop();
				stage_2_disk_cost += fm->getDiscReadCount();
			}
			run_readers.clear();

		}
		for (; !new_run_files_names.empty(); new_run_files_names.pop())
			run_files_names.push(new_run_files_names.front());
		std::cout << "Cycle " << *cycle_count << " done\n";
	} while (run_files_names.size() != 1);
	return stage_2_disk_cost;
}

void StrukturyBazDanych::sortUsingLargeBuffers(const char* filePath, int buffer_count, size_t block_size, bool print_debug)
{
	long long unsigned int stage_1_disk_cost = 0;
	long long unsigned int stage_2_disk_cost = 0;
	long long unsigned int cycle_count = 0;

	int b = block_size / sizeof(Record);

	long int nb = buffer_count * b;

	unsigned long long int file_count = 0;
	std::queue<std::string> run_files_names;
	std::stack<std::string> files_to_delete;


	//	Stage 0. (Creating the BIG BUFFERS)
	// they will take up a big connected chunk of memory

	std::byte* buffer_space = new std::byte[block_size * buffer_count];
	Buffer** buffers = new Buffer * [buffer_count];
	for (int i = 0; i < buffer_count; i++)
	{
		buffers[i] = new Buffer(buffer_space + i * block_size, block_size);
	}

	std::cout << "\nSORTING WITH LARGE BUFFERS\n\n";

	printRecordNumber(filePath);

	if (print_debug)
	{
		std::cout << "Sorting file " << filePath << "\nusing n=" << buffer_count << " buffers\n"
			<< "of size " << block_size << " bytes\n"
			<< "giving blocking factor b=" << b << "\n";
	}

	std::cout << "\nStage one: Creating runs\n";
	stage_1_disk_cost = divideIntoRuns(filePath, block_size, nb, buffer_space, run_files_names, files_to_delete, &file_count);
	std::cout << "Stage one finished!\n";

	
	
	//	Stage 2. (Merging)

	std::cout << "Stage two: Merging\n";

	stage_2_disk_cost = mergeUntilOneFile(&cycle_count, run_files_names, files_to_delete, &file_count, block_size, buffers, buffer_count);

	std::cout << "Done!\n";
	
	delete[] buffers;
	delete[] buffer_space;

	std::string new_file_name = std::string("sorted_") + std::string(filePath);
	files_to_delete.pop();
	rename(run_files_names.front().c_str(), new_file_name.c_str());

	for (; !files_to_delete.empty(); files_to_delete.pop())
	{
		remove(files_to_delete.top().c_str());
	}
	
	if (askUser("Do you want to see the sorted records?"))
		printOutFile(new_file_name.c_str());
	if (askUser("Do you want to count the sorted records?"))
		printRecordNumber(new_file_name.c_str());

	std::cout << "\nCycles needed: " << cycle_count << "\n";
	std::cout << "Total stage 1  disk cost: " << stage_1_disk_cost << "\n";
	std::cout << "Total stage 2  disk cost: " << stage_2_disk_cost << "\n";
	std::cout << "Total disk cost: " << stage_1_disk_cost + stage_2_disk_cost << "\n";
}

bool StrukturyBazDanych::askUser(const char* question)
{
	char user_input;
	std::cout << question << " [y/n]>";
	std::cin >> user_input;
	if (user_input == 'y' or user_input == 'Y')
		return true;
	return false;
}
