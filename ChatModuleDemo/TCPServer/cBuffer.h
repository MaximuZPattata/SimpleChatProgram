#pragma once

#include "pch.h"

class cBuffer
{
private:

	int mWriteIndex;
	int mReadIndex;

public:

	std::vector<uint8_t> mBufferData;

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