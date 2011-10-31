#if !defined(MPEG1_UTILITY_H)
#define MPEG1_UTILITY_H

static inline void copyShorts(const short *source, int sourceIndex, short *destination, int destinationIndex, int count)
{
	while(count--)
		destination[destinationIndex++] = source[sourceIndex++];
}

static inline void copyInts(const int *source, int sourceIndex, int *destination, int destinationIndex, int count)
{
	while(count--)
		destination[destinationIndex++] = source[sourceIndex++];
}

#endif
