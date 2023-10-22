#include "pch.h"
#include "cBuffer.h"

cBuffer::cBuffer(int size = 512)
{
	mBufferData.resize(size);
	mWriteIndex = 0;
	mReadIndex = 0;
}

cBuffer::~cBuffer()
{ }

uint16_t cBuffer::ReadUInt16BE()
{
	uint16_t value = 0;

	value |= mBufferData[mReadIndex++] << 8;
	value |= mBufferData[mReadIndex++];

	return value;
}

uint32_t cBuffer::ReadUInt32BE()
{
	uint32_t value = 0;

	value |= mBufferData[mReadIndex++] << 24;
	value |= mBufferData[mReadIndex++] << 16;
	value |= mBufferData[mReadIndex++] << 8;
	value |= mBufferData[mReadIndex++];

	return value;
}

std::string cBuffer::ReadString(uint32_t length)
{
	std::string str;

	for (int i = 0; i < length; i++)
	{
		str.push_back(mBufferData[mReadIndex++]);
	}

	return str;
}

void cBuffer::WriteUInt16BE(uint16_t value)
{
	mBufferData[mWriteIndex++] = value >> 8;
	mBufferData[mWriteIndex++] = value;
}

void cBuffer::WriteUInt32BE(uint32_t value)
{
	mBufferData[mWriteIndex++] = value >> 24;
	mBufferData[mWriteIndex++] = value >> 16;
	mBufferData[mWriteIndex++] = value >> 8;
	mBufferData[mWriteIndex++] = value;
}

void cBuffer::WriteString(const std::string& str)
{
	int strLength = str.length();
	for (int i = 0; i < strLength; i++)
	{
		mBufferData[mWriteIndex++] = str[i];
	}
}

void cBuffer::ReinitializeIndex()
{
	mWriteIndex = 0;
	mReadIndex = 0;
}

void cBuffer::ClearBuffer()
{
	ReinitializeIndex();

	mBufferData.clear();
	mBufferData.resize(512);
}
