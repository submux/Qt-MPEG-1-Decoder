#include "inputbitstream.h"

namespace Mpeg1
{
	int InputBitstream::s_mask[] = {
		0x00000000, 0x00000001, 0x00000003, 0x00000007,
		0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
		0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
		0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
		0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
		0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
		0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
		0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
		0xffffffff
	};

	int InputBitstream::s_complementMask[] = {
		0xffffffff, 0xfffffffe, 0xfffffffc, 0xfffffff8,
		0xfffffff0, 0xffffffe0, 0xffffffc0, 0xffffff80,
		0xffffff00, 0xfffffe00, 0xfffffc00, 0xfffff800,
		0xfffff000, 0xffffe000, 0xffffc000, 0xffff8000,
		0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000,
		0xfff00000, 0xffe00000, 0xffc00000, 0xff800000,
		0xff000000, 0xfe000000, 0xfc000000, 0xf8000000,
		0xf0000000, 0xe0000000, 0xc0000000, 0x80000000,
		0x00000000
	};

	int InputBitstream::s_signMask[] = {
		0x00000000, 0x00000001, 0x00000002, 0x00000004,
		0x00000008, 0x00000010, 0x00000020, 0x00000040,
		0x00000080, 0x00000100, 0x00000200, 0x00000400,
		0x00000800, 0x00001000, 0x00002000, 0x00004000,
		0x00008000, 0x00010000, 0x00020000, 0x00040000,
		0x00080000, 0x00100000, 0x00200000, 0x00400000,
		0x00800000, 0x01000000, 0x02000000, 0x04000000,
		0x08000000, 0x10000000, 0x20000000, 0x40000000,
		0x80000000
	};

	InputBitstream::InputBitstream(QIODevice *input) :
		m_input(input),
		m_bufferLength(BufferSize),
		m_bufferIndex(BufferSize << 3)
	{
		m_buffer = new quint8[BufferSize];
	}

	InputBitstream::~InputBitstream()
	{
		delete [] m_buffer;
	}

	void InputBitstream::close()
	{
		m_input->close();
	}

	void InputBitstream::fillBuffer()
	{
		int byteOffset = m_bufferIndex >> 3;
		int bytesLeft  = m_bufferLength - byteOffset;

		// Move remaining bytes to the beginning of the buffer
		memcpy(m_buffer, m_buffer + byteOffset, bytesLeft);

		// Note: bytesLeft and byteOffset are interchanged due 
		// to the above buffer relocation
		int length = m_input->read((char *)(m_buffer + bytesLeft), byteOffset);

		if (length < byteOffset)
			m_bufferLength = bytesLeft + length;

		m_bufferIndex &= 0x7;
	}

	/*
	* Peek "count" bits without removing them from the buffer
	*/
	int InputBitstream::nextBits(int count)
	{
		int value = 0;

		if(m_bufferIndex + count > m_bufferLength << 3)
			fillBuffer();

		int byteOffset = m_bufferIndex >> 3;

		int end = (m_bufferIndex + count - 1) >> 3;  // End byte position
		int room = 8 - (m_bufferIndex & 0x7);           // Room in current byte

		if (room >= count) {
			value = (m_buffer[byteOffset] >> (room - count)) & s_mask[count];
			return value;
		}

		int leftover = (m_bufferIndex + count) & 0x7;        // Leftover bits in last byte

		value |= m_buffer[byteOffset] & s_mask[room];  // Fill out first byte

		for (byteOffset++; byteOffset < end; byteOffset++) 
		{
			value <<= 8;                            // Shift and
			value |= m_buffer[byteOffset] & s_mask[8]; // Put next byte
		}

		if (leftover > 0) 
		{
			value <<= leftover;                     // Make room for remaining bits
			value |= (m_buffer[byteOffset] >> (8 - leftover)) & s_mask[leftover];
		}
		else 
		{
			value <<= 8;
			value |= m_buffer[byteOffset] & s_mask[8];
		}

		return value;
	}

	int InputBitstream::nextSignedBits(int count)
	{
		int value = nextBits(count);

		if ((s_signMask[count] & value) != 0)
			return value | s_complementMask[count];
		else
			return value;
	}

	int InputBitstream::getBits(int count)
	{
		int value = nextBits(count);
		m_bufferIndex += count;
		return value;
	}

	int InputBitstream::getSignedBits(int count)
	{
		int value = nextSignedBits(count);
		m_bufferIndex += count;

		return value;
	}

	bool InputBitstream::isByteAligned() const
	{
		return (m_bufferIndex & 0x7) == 0;
	}
}
