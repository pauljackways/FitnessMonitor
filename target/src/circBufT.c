// *******************************************************
// 
// circBufT.c
//
// Support for a circular buffer of uint32_t values on the 
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  8.3.2017
// 
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"
#include <stdbool.h>

// *******************************************************
// global variable for determining full or empty state
int32_t spaceLeft;

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
int32_t *
initCircBuf (circBuf_t *buffer, uint32_t size)
{
	if (size > MAX_CIRCBUFT_SIZE || size <= 0) {
		return NULL;
	}
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (int32_t *) calloc (size, sizeof(int32_t));
	spaceLeft = buffer->size;
	return buffer->data;
}

// Note use of calloc() to clear contents.

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void
writeCircBuf (circBuf_t *buffer, int32_t entry)
{
	if (spaceLeft <= 0) {
		return;
	}
	buffer->data[buffer->windex] = entry;
	spaceLeft--;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function checks
// if reading has advanced ahead of writing.


int32_t
readCircBuf (circBuf_t *buffer)
{
	int32_t entry;
	if (spaceLeft == buffer->size) {
		entry = 0;
	} else {
		entry = buffer->data[buffer->rindex];
		spaceLeft++;
		buffer->rindex++;
		if (buffer->rindex >= buffer->size)
		buffer->rindex = 0;
	}
    return entry;
}

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and other fields to 0. The buffer can
// re-initialised by another call to initCircBuf().
void
freeCircBuf (circBuf_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
}

