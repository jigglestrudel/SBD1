#include "Buffer.h"

Buffer::Buffer(size_t size)
{
	this->buffer_p = new std::byte[size];
	this->buffer_cursor = 0;
	this->buffer_size = size;
	this->foreign_pointer = false;

}

Buffer::Buffer(std::byte* foreign_pointer, size_t size)
{
	this->buffer_p = foreign_pointer;
	this->buffer_cursor = 0;
	this->buffer_size = size;
	this->foreign_pointer = true;
}

Buffer::~Buffer()
{
	if (!foreign_pointer)
		delete buffer_p;
}

std::byte* Buffer::getBufferPointer()
{
	return buffer_p;
}

std::byte* Buffer::getMovedBufferPointer()
{
	return buffer_p + buffer_cursor;
}

size_t Buffer::getBufferCursor()
{
	return buffer_cursor;
}

void Buffer::resetBufferCursor()
{
	buffer_cursor = 0;
}

void Buffer::moveBufferCursor(size_t offset)
{
	buffer_cursor += offset;
	if (buffer_cursor > buffer_size)
		throw BufferException();
}

size_t Buffer::getSpaceLeft()
{
	return buffer_size - buffer_cursor;
}

size_t Buffer::getSize()
{
	return buffer_size;
}

bool Buffer::canFit(size_t stuff_size)
{
	return buffer_cursor + stuff_size <= buffer_size;
}
