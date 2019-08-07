#pragma once
#include "Base.h"

typedef struct {
	unsigned char type;
	unsigned char channel;
	unsigned char data[2];
} MIDIMSG;

struct MIDI_BUFFER
{
	unsigned char opcode;
	unsigned char channel;
	unsigned char data[2];

	unsigned char type;
	unsigned int channel_mask; 

	void *userdata;

	unsigned char pos;
	unsigned char readlen;
};

#define MIDI_NOTE_OFF           0x80
#define MIDI_NOTE_ON            0x90
#define MIDI_POLY_PRESSURE      0xa0
#define MIDI_CONTROL_CHANGE     0xb0
#define MIDI_PROGRAM_CHANGE     0xc0
#define MIDI_CHANNEL_PRESSURE   0xd0
#define MIDI_PITCH_WHEEL        0xe0

inline
void
ParseMidi(
	void* pData,
	size_t DataSize
)
{

}
