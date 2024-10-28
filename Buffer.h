#pragma once
#include <cstdlib>
#include <cstddef>
#include <exception>

class Buffer
{
public:

	Buffer(size_t size);
	Buffer(std::byte* foreign_pointer, size_t size);
	~Buffer();

	std::byte* getBufferPointer();
	std::byte* getMovedBufferPointer();
	size_t getBufferCursor();
	void resetBufferCursor();
	void moveBufferCursor(size_t offset);
	size_t getSpaceLeft();
	size_t getSize();

	bool canFit(size_t stuff_size);

private:
	std::byte* buffer_p;
	size_t buffer_size;
	size_t buffer_cursor;

	bool foreign_pointer;

};


struct BufferException : public std::exception {
	const char* what() const throw () {
		return "FileManager exception";
	}
};
