#include "FileManager.h"


FileManager::FileManager(const char* filePath, size_t size)
{
	if (size < sizeof(Record))
	{
		std::cerr << "FileManager::FileManager : Block size smaller than one record\n";
		throw FileManagerException();
	}

	this->filePath = filePath;
	this->data_block_p = new std::byte[size];
	this->state = OFF;
	this->data_block_size = size;
	this->data_block_cursor = 0;
	this->size_read = 0;
	this->was_eof_reached = false;
	this->end_of_records = false;
}

FileManager::~FileManager()
{
	if (this->state != OFF)
	{
		std::cerr << "FileManager::~FileManager : FileManager not off!\n";

	}
	delete data_block_p;
}

void FileManager::readRecord(Record* dest)
{
	if (this->state != READING)
	{
		std::cerr << "FileManager::readRecord : FileManager not in reading state!\n";
		throw FileManagerException();
	}

	if (this->data_block_cursor == this->size_read)
	{
		this->readBlock();
	}
	else if (this->data_block_cursor > this->size_read)
	{
		std::cerr << "FileManager::readRecord : Cursor out of bounds\n";
		throw FileManagerException();
	}

	if (this->size_read < sizeof(Record))
	{
		std::cerr << "FileManager::readRecord : Read size smaller than one record\n";
		throw FileManagerException();
	}

	// While reading there are 2 cases

	// case 1.	Block has enough bytes to read a record

	if (this->size_read - this->data_block_cursor >= sizeof(Record) )
	{
		this->copyToAndMoveCursor(dest, sizeof(Record));
	}

	// case 2.	Current block doesn't have enough bytes 
	
	//			We need to read in another block
	//			However for this scenario we need to have read a full block beforehand

	else if (!this->was_eof_reached and this->size_read - this->data_block_cursor < sizeof(Record))
	{
		size_t bytes_prev = this->size_read - this->data_block_cursor;
		size_t bytes_next = sizeof(Record) - bytes_prev;

		this->copyToAndMoveCursor(dest, bytes_prev);
		this->readBlock();
		this->copyToAndMoveCursor(dest + bytes_prev, bytes_next);
	}
	else if (this->was_eof_reached and this->size_read - this->data_block_cursor < sizeof(Record))
	{
		std::cerr << "FileManager::readRecord : Not enough bytes for a record\n";
		throw FileManagerException();
	}

	if (this->was_eof_reached and this->data_block_cursor == this->size_read)
	{
		this->end_of_records = true;
	}
}

void FileManager::writeRecord(Record* source)
{
	if (this->state != WRITING)
	{
		std::cerr << "FileManager::writeRecord : FileManager not in writing state!\n";
		throw FileManagerException();
	}

	if (this->data_block_cursor > this->data_block_size)
	{
		std::cerr << "FileManager::writeRecord : Cursor out of bounds\n";
		throw FileManagerException();
	}

	if (this->data_block_cursor + sizeof(Record) <= this->data_block_size)
	{
		this->copyFromAndMoveCursor(source, sizeof(Record));
	}
	else
	{
		size_t bytes_prev = this->data_block_size - this->data_block_cursor;
		size_t bytes_next = sizeof(Record) - bytes_prev;
		
		this->copyFromAndMoveCursor(source, bytes_prev);
		this->writeBlock();
		this->copyFromAndMoveCursor(source + bytes_prev, bytes_next);

	}

}

void FileManager::startReading()
{
	this->openFileStream(READING);
	this->data_block_cursor = 0;
	this->size_read = 0;
	this->was_eof_reached = false;
	this->end_of_records = false;
}

void FileManager::startWriting()
{
	this->openFileStream(WRITING);
	this->data_block_cursor = 0;
}

void FileManager::finishAndStop()
{
	if (this->state == OFF)
	{
		std::cerr << "FileManager::finishAndStop : FileManager not on!\n";
		throw FileManagerException();
	}

	if (this->state == READING)
	{
		if (data_block_cursor != size_read)
			std::cerr << "FileManager::finishAndStop : Not whole block processed yet\n";
	}
	if (this->state == WRITING)
	{
		writeBlockTillCursor();
	}
	
	closeFileStream();
}

FileManagerState FileManager::getState()
{
	return this->state;
}

bool FileManager::wasEndOfRecordsReached()
{
	return this->end_of_records;
}

void FileManager::openFileStream(FileManagerState desired_state)
{
	if (this->state != OFF)
	{
		std::cerr << "FileManager::openFileStream : FileStream already opened!\n";
		throw FileManagerException();
	}

	if (desired_state != READING and desired_state != WRITING)
	{
		std::cerr << "FileManager::openFileStream : Wrong desired FileManagerState!\n";
		throw FileManagerException();
	}
	if (desired_state == READING)
	{
		this->file_stream = std::fstream(filePath, std::ios_base::in | std::ios_base::binary);
		if (this->file_stream.fail())
		{
			std::cerr << "FileManager::openFileStream : FileStream failed to open!\n";
			throw FileManagerException();
		}
		this->state = READING;
	}
	else if (desired_state == WRITING)
	{
		this->file_stream = std::fstream(filePath, std::ios_base::out | std::ios_base::binary);
		if (this->file_stream.fail())
		{
			std::cerr << "FileManager::openFileStream : FileStream failed to open!\n";
			throw FileManagerException();
		}
		this->state = WRITING;
	}
}

void FileManager::closeFileStream()
{
	if (this->state == OFF)
	{
		std::cerr << "FileManager::closeFileStream : FileStream already closed!\n";
		throw FileManagerException();
	}

	if (this->file_stream.is_open())
	{
		this->file_stream.close();
	}

	this->data_block_cursor = 0;
	this->state = OFF;
}

void FileManager::readBlock()
{
	if (this->state != READING)
	{
		std::cerr << "FileManager::readBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	if (data_block_cursor != size_read)
		std::cerr << "FileManager::readBlock : Not whole block processed yet\n";

	this->file_stream.read((char*)this->data_block_p, this->data_block_size);
	this->was_eof_reached = false;
	this->size_read = this->data_block_size;
	this->data_block_cursor = 0;

	if (this->file_stream.eof())
	{
		this->was_eof_reached = true;
		this->size_read = this->file_stream.gcount();
	}
	else if (!this->file_stream.good())
	{
		this->size_read = this->file_stream.gcount();
		std::cerr << "FileManager::readBlock : reading failed!\n";
		throw FileManagerException();
	}
}

void FileManager::writeBlock()
{
	if (this->state != WRITING)
	{
		std::cerr << "FileManager::writeBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	this->file_stream.write((char*)this->data_block_p, this->data_block_size);
	this->data_block_cursor = 0;
	if (!this->file_stream.good())
	{
		std::cerr << "FileManager::writeBlock : writing failed!\n";
		throw FileManagerException();
	}
}

void FileManager::writeBlockTillCursor()
{
	if (this->state != WRITING)
	{
		std::cerr << "FileManager::writeBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	this->file_stream.write((char*)this->data_block_p, this->data_block_cursor);
	this->data_block_cursor = 0;
	if (!this->file_stream.good())
	{
		std::cerr << "FileManager::writeBlock : writing failed!\n";
		throw FileManagerException();
	}
}

void FileManager::copyToAndMoveCursor(void* dest, size_t count)
{
	if (this->data_block_cursor + count > this->size_read)
	{
		std::cerr << "FileManager::copyToAndMoveCursor : not enough bytes left in block\n";
		throw FileManagerException();
	}

	memcpy(dest, this->data_block_p + this->data_block_cursor, count);
	this->data_block_cursor += count;
}

void FileManager::copyFromAndMoveCursor(void* src, size_t count)
{
	if (this->data_block_cursor + count > this->data_block_size)
	{
		std::cerr << "FileManager::copyFromAndMoveCursor : not enough free bytes left in block\n";
		throw FileManagerException();
	}

	memcpy(this->data_block_p + this->data_block_cursor, src, count);
	this->data_block_cursor += count;
}
