#include <cstdlib>
#include <ctime>

#include "FileManager.h"
#include "Record.h"

constexpr long int rand_max = 10000L;
constexpr double upper_bound = 100.;

constexpr size_t FOUR_KILOBYTES = 4096;
constexpr size_t TWO_KILOBYTES = 2048;
constexpr size_t ONE_KILOBYTE = 1024;

void generateRandomRecords(const char* filePath, int n);
void generateRecordsFromUser(const char* filePath, int n);
void printOutFile(const char* filePath);
void copyFileToFileByRecord(const char* src, const char* dest);

class StrukturyBazDanych 
{
public:
	StrukturyBazDanych();
	~StrukturyBazDanych();

	void static sortUsingLargeBuffers(const char* filePath, int buffer_number, size_t block_size, bool print_debug);


private:

};

