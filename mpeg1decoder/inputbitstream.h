#if !defined(MPEG1_INPUTBITSTREAM_H)
#define MPEG1_INPUTBITSTREAM_H

#include <QtCore/QIODevice>

namespace Mpeg1
{
	class InputBitstream
	{
		static const int BufferSize = 16384;
	public:
		InputBitstream(QIODevice *input);

		~InputBitstream();

		void close();

		int nextBits(int count);

		int nextSignedBits(int count);

		int getBits(int count);

		int getSignedBits(int count);

		bool isByteAligned() const;

	private:
		void fillBuffer();

	private:
		QIODevice *m_input;
		quint8 *m_buffer;
		int m_bufferLength;
		int m_bufferIndex;

		static int s_mask[];
		static int s_complementMask[];
		static int s_signMask[];
	};
}

#endif
