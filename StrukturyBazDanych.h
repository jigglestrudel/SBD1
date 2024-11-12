#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
#include <stack>

#include "FileManager.h"
#include "Record.h"
#include "TournamentTree.h"

constexpr long int rand_max = 10000L;
constexpr double upper_bound = 100.;

constexpr size_t FOUR_KILOBYTES = 4096;
constexpr size_t TWO_KILOBYTES = 2048;
constexpr size_t ONE_KILOBYTE = 1024;
constexpr size_t HALF_KILOBYTE = 512;



static class StrukturyBazDanych 
{
public:
	StrukturyBazDanych();
	~StrukturyBazDanych();

	void runTUI();

	void static generateRandomRecords(const char* filePath, int n);
	void static generateOrderedRecords(const char* filePath, int n);
	void static generateRecordsFromUser(const char* filePath, int n);
	void static printOutFile(const char* filePath);
	void static printRecordNumber(const char* filePath);
	void static copyFileToFileByRecord(const char* src, const char* dest);
	long long unsigned int static divideIntoRuns(const char* filePath, size_t block_size, long int nb, std::byte* buffer_space, std::queue<std::string>& run_files_names, std::stack<std::string>& files_to_delete, long long unsigned int* file_count);
	long long unsigned int static mergeRunsToFile(std::string file_name, size_t block_size, Buffer** buffers, int buffer_count, std::vector<FileManager*> run_readers, int runs_opened);
	long long unsigned int static mergeUntilOneFile(long long unsigned int* cycle_count, std::queue<std::string>& run_files_names, std::stack<std::string>& files_to_delete, long long unsigned int* file_count, size_t block_size, Buffer** buffers, int buffer_count);
	void static sortUsingLargeBuffers(const char* filePath, int buffer_number, size_t block_size, bool print_debug);
	bool static askUser(const char* question);


};

