#include "pch.h"
#include "cBuffer.h"

// Constructor for the cBuffer class.
// [param_1]: The size of the buffer (default: 512 bytes).
cBuffer::cBuffer(int size = 512)
{
	mBufferData.resize(size);
	mWriteIndex = 0;
	mReadIndex = 0;
}

// Destructor for the cBuffer class.
cBuffer::~cBuffer()
{ }

// Function to read a 16-bit unsigned short in big-endian format from the buffer.
// [return_value]: The 16-bit unsigned integer value read is returned.
uint16_t cBuffer::ReadUInt16BE()
{
	uint16_t value = 0;

	value |= mBufferData[mReadIndex++] << 8;
	value |= mBufferData[mReadIndex++];

	return value;
}

// Function to read a 32-bit unsigned integer in big-endian format to the buffer.
// [return_value]: The 32-bit unsigned integer value read is returned.
uint32_t cBuffer::ReadUInt32BE()
{
	uint32_t value = 0;

	value |= mBufferData[mReadIndex++] << 24;
	value |= mBufferData[mReadIndex++] << 16;
	value |= mBufferData[mReadIndex++] << 8;
	value |= mBufferData[mReadIndex++];

	return value;
}

// Function to read a string from the buffer.
// [param_1]: The 32-bit unsigned integer of the string length.
// [return_value]: The string value read is returned.
std::string cBuffer::ReadString(uint32_t length)
{
	std::string str;

	for (int i = 0; i < length; i++)
	{
		str.push_back(mBufferData[mReadIndex++]);
	}

	return str;
}

// Function to write a 16-bit unsigned short in big-endian format to the buffer.
// [param_1]: The 16-bit unsigned short to write.
void cBuffer::WriteUInt16BE(uint16_t value)
{
	mBufferData[mWriteIndex++] = value >> 8;
	mBufferData[mWriteIndex++] = value;
}

// Function to write a 32-bit unsigned integer in big-endian format to the buffer.
// [param_1]: The 32-bit unsigned integer to write.
void cBuffer::WriteUInt32BE(uint32_t value)
{
	mBufferData[mWriteIndex++] = value >> 24;
	mBufferData[mWriteIndex++] = value >> 16;
	mBufferData[mWriteIndex++] = value >> 8;
	mBufferData[mWriteIndex++] = value;
}

// Function to write a string to the buffer.
// [param_1]: The string to write.
void cBuffer::WriteString(const std::string& str)
{

	int strLength = str.length();
	for (int i = 0; i < strLength; i++)
	{
		mBufferData[mWriteIndex++] = str[i];
	}
}

// Function to reinitialize the read and write indices to zero.
void cBuffer::ReinitializeIndex()
{
	mWriteIndex = 0;
	mReadIndex = 0;
}

// Function to empty the buffer
void cBuffer::ClearBuffer()
{
	ReinitializeIndex();

	mBufferData.clear();
	mBufferData.resize(512);
}
