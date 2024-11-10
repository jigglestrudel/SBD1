#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <queue>

#include "FileManager.h"
#include "Record.h"
#include "TournamentTree.h"

constexpr long int rand_max = 10000L;
constexpr double upper_bound = 100.;

constexpr size_t FOUR_KILOBYTES = 4096;
constexpr size_t TWO_KILOBYTES = 2048;
constexpr size_t ONE_KILOBYTE = 1024;



static class StrukturyBazDanych 
{
public:

	void static generateRandomRecords(const char* filePath, int n);
	void static generateOrderedRecords(const char* filePath, int n);
	void static generateRecordsFromUser(const char* filePath, int n);
	void static printOutFile(const char* filePath);
	void static copyFileToFileByRecord(const char* src, const char* dest);
	void static sortUsingLargeBuffers(const char* filePath, int buffer_number, size_t block_size, bool print_debug);
};

