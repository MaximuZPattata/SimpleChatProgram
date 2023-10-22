#pragma once

#include "pch.h"

// This class is created to manage the buffer for reading and writing data(serializing and deserializing)
class cBuffer
{
private:

	int mWriteIndex; // Index for writing data to the buffer
	int mReadIndex;	 // Index for reading data from the buffer

public:

	std::vector<uint8_t> mBufferData;  // Container to store binary data

	cBuffer(int size);
	~cBuffer();

	uint16_t ReadUInt16BE();
	uint32_t ReadUInt32BE();
	std::string ReadString(uint32_t length);

	void WriteUInt16BE(uint16_t value);
	void WriteUInt32BE(uint32_t value);
	void WriteString(const std::string& str);

	void ReinitializeIndex();
	void ClearBuffer();

};