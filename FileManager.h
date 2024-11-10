#pragma once
#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>

#include "Record.h"
#include "Buffer.h"


enum FileManagerState
{
	READING,
	WRITING,
	OFF
};


class FileManager
{
public:
	FileManager(const char* filePath, size_t size, Buffer* buffer = nullptr);
	~FileManager();

	void readRecord(Record* dest);
	void writeRecord(Record* source);

	void startReading();
	void startWriting();

	void finishAndStop();
	FileManagerState getState();
	bool wasEndOfRecordsReached();

	void changeBuffer(Buffer* buffer);

	long long unsigned int getDiscReadCount();
	long long unsigned int getDiscWriteCount();

private:
	const char* filePath;
	std::fstream file_stream;

	Buffer* buffer;
	bool is_buffer_foreign;

	long long unsigned int disc_read_count;
	long long unsigned int disc_write_count;

	FileManagerState state;
	bool was_eof_reached;
	size_t size_read;
	bool end_of_records;

	void openFileStream(FileManagerState state);
	void closeFileStream();
	void readBlock();
	void writeBlock();
	void writeBlockTillCursor();
	void copyToAndMoveCursor(void* dest, size_t count);
	void copyFromAndMoveCursor(void* src, size_t count);

};

struct FileManagerException : public std::exception {
	const char* what() const throw () {
		return "FileManager exception";
	}
};

struct FileManagerNoRecords : FileManagerException {
	const char* what() const throw () {
		return "FileManagerNoRecords exception";
	}
};
