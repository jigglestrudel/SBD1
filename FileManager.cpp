#include "FileManager.h"


FileManager::FileManager(const char* filePath, size_t size, Buffer* buffer)
{
	if (size < sizeof(Record))
	{
		std::cerr << "FileManager::FileManager : Block size smaller than one record\n";
		throw FileManagerException();
	}

	this->filePath = filePath;
	this->state = OFF;
	
	if (buffer == nullptr)
	{
		this->buffer = new Buffer(size);
		this->is_buffer_foreign = false;
	}
	else
	{
		this->is_buffer_foreign = true;
		this->buffer = buffer;
	}
	this->size_read = 0;
	this->was_eof_reached = false;
	this->end_of_records = false;

	this->disc_read_count = 0;
	this->disc_write_count = 0;
}

FileManager::~FileManager()
{
	if (this->state != OFF)
	{
		std::cerr << "FileManager::~FileManager : FileManager not off!\n";

	}
	
	if (!is_buffer_foreign)
		delete buffer;
}

void FileManager::readRecord(Record* dest)
{
	if (this->state != READING)
	{
		std::cerr << "FileManager::readRecord : FileManager not in reading state!\n";
		throw FileManagerException();
	}

	if (buffer->getBufferCursor() == this->size_read)
	{
		this->readBlock();
	}
	else if (buffer->getBufferCursor() > this->size_read)
	{
		std::cerr << "FileManager::readRecord : Cursor out of bounds\n";
		throw FileManagerException();
	}

	if (this->size_read < sizeof(Record))
	{
		//std::cerr << "FileManager::readRecord : Read size smaller than one record\n";
		this->end_of_records = true;
		throw FileManagerNoRecords();
	}

	// While reading there are 2 cases

	// case 1.	Block has enough bytes to read a record

	if (this->size_read - buffer->getBufferCursor() >= sizeof(Record) )
	{
		this->copyToAndMoveCursor(dest, sizeof(Record));
	}

	// case 2.	Current block doesn't have enough bytes 
	
	//			We need to read in another block
	//			However for this scenario we need to have read a full block beforehand

	else if (!this->was_eof_reached and this->size_read - buffer->getBufferCursor() < sizeof(Record))
	{
		size_t bytes_prev = this->size_read - buffer->getBufferCursor();
		size_t bytes_next = sizeof(Record) - bytes_prev;

		this->copyToAndMoveCursor(dest, bytes_prev);
		this->readBlock();
		this->copyToAndMoveCursor((std::byte*)dest + bytes_prev, bytes_next);
	}
	else if (this->was_eof_reached and this->size_read - buffer->getBufferCursor() < sizeof(Record))
	{
		std::cerr << "FileManager::readRecord : Not enough bytes for a record\n";
		throw FileManagerException();
	}

	if (this->was_eof_reached and buffer->getBufferCursor() == this->size_read)
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

	if (buffer->canFit(sizeof(Record)))
	{
		this->copyFromAndMoveCursor(source, sizeof(Record));
	}
	else
	{
		size_t bytes_prev = buffer->getSpaceLeft();
		size_t bytes_next = sizeof(Record) - bytes_prev;
		
		this->copyFromAndMoveCursor(source, bytes_prev);
		this->writeBlock();
		this->copyFromAndMoveCursor((std::byte*)source + bytes_prev, bytes_next);

	}

}

void FileManager::startReading()
{
	this->openFileStream(READING);
	buffer->resetBufferCursor();
	this->size_read = 0;
	this->was_eof_reached = false;
	this->end_of_records = false;
}

void FileManager::startWriting()
{
	this->openFileStream(WRITING);
	buffer->resetBufferCursor();
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
		if (this->buffer->getBufferCursor() != size_read)
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

void FileManager::changeBuffer(Buffer* buffer)
{
	if (!this->is_buffer_foreign)
		delete this->buffer;
	this->is_buffer_foreign = true;
	this->buffer = buffer;
}

long long unsigned int FileManager::getDiscReadCount()
{
	return this->disc_read_count;
}

long long unsigned int FileManager::getDiscWriteCount()
{
	return this->disc_write_count;
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

	buffer->resetBufferCursor();
	this->state = OFF;
}

void FileManager::readBlock()
{
	this->disc_read_count++;

	if (this->state != READING)
	{
		std::cerr << "FileManager::readBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	if (buffer->getBufferCursor() != size_read)
		std::cerr << "FileManager::readBlock : Not whole block processed yet\n";

	buffer->resetBufferCursor();
	this->file_stream.read((char*)(buffer->getBufferPointer()), buffer->getSize());
	this->was_eof_reached = false;
	this->size_read = buffer->getSize();
	buffer->resetBufferCursor();

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
	this->disc_write_count++;

	if (this->state != WRITING)
	{
		std::cerr << "FileManager::writeBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	buffer->resetBufferCursor();
	this->file_stream.write((char*)(buffer->getBufferPointer()), buffer->getSize());
	if (!this->file_stream.good())
	{
		std::cerr << "FileManager::writeBlock : writing failed!\n";
		throw FileManagerException();
	}
}

void FileManager::writeBlockTillCursor()
{
	if (buffer->getBufferCursor() != 0)
		this->disc_write_count++;

	if (this->state != WRITING)
	{
		std::cerr << "FileManager::writeBlock : Wrong FileManagerState\n";
		throw FileManagerException();
	}

	this->file_stream.write((char*)buffer->getBufferPointer(), buffer->getBufferCursor());
	buffer->resetBufferCursor();
	if (!this->file_stream.good())
	{
		std::cerr << "FileManager::writeBlock : writing failed!\n";
		throw FileManagerException();
	}
}

void FileManager::copyToAndMoveCursor(void* dest, size_t count)
{
	if (buffer->getBufferCursor() + count > this->size_read)
	{
		std::cerr << "FileManager::copyToAndMoveCursor : not enough bytes left in block\n";
		throw FileManagerException();
	}

	memcpy(dest, buffer->getMovedBufferPointer(), count);
	buffer->moveBufferCursor(count);
}

void FileManager::copyFromAndMoveCursor(void* src, size_t count)
{
	if (!buffer->canFit(count))
	{
		std::cerr << "FileManager::copyFromAndMoveCursor : not enough free bytes left in block\n";
		throw FileManagerException();
	}

	memcpy(buffer->getMovedBufferPointer(), src, count);
	buffer->moveBufferCursor(count);
}
