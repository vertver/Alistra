#pragma once
#include "Base_Sound.h"

typedef struct
{
	i32 NoteIndex;
	i32 NoteFrames;
	i32 NoteVelocity;
	f32 NoteVolume;
} NOTE_STRUCT;

struct MidiParser {
	u8*     DataIn = nullptr;
	u32     DataLen = 0;
	u32     DataPos = 0;

	bool     ReadEof() {
		if (DataPos >= DataLen) return true;
		return false;
	}

	void*   ReadStructPtr(u32 Size) {
		if (DataPos >= DataLen) return nullptr;
		if (DataPos + Size >= DataLen) return nullptr;
		void* Ptr = &DataIn[DataPos];
		DataPos += Size;
		return Ptr;
	}

	u8      ReadU8() {
		if (DataPos >= DataLen) return 0;
		return DataIn[DataPos++];
	}

	void    ReadStr(u8* Data, u32 Len) {
		for (u32 i = 0; i < Len; i++) {
			Data[i] = ReadU8();
		}
		Data[Len] = 0;
	}

	u16     ReadU16() {
		return  (ReadU8() << 8) | ReadU8();
	}

	u32     ReadU24() {
		return  (ReadU8() << 16) | (ReadU8() << 8) | ReadU8();
	}

	u32     ReadU32() {
		return  (ReadU8() << 24) | (ReadU8() << 16) | (ReadU8() << 8) | ReadU8();
	}

	u32     ReadLen() {
		u32 value;
		u8  c;
		if ((value = ReadU8()) & 0x80) {
			value &= 0x7f;
			do {
				value = (value << 7) + ((c = ReadU8()) & 0x7f);
			} while (c & 0x80);
		}
		return value;
	}

	void    Parse(void* MidiFileData, u32 MidiFileLen) {
		DataIn = (u8*)MidiFileData;
		DataLen = MidiFileLen;
		DataPos = 0;

		u32 Marker = ReadU32();
		u32 Len = ReadU32();

		u16 TypeofSMF = ReadU16();
		u16 NumberofTracks = ReadU16();
		u16 PPQ = ReadU16();

		Begin(NumberofTracks, PPQ);

		u32 TrackID = 0;
		while (!ReadEof()) {
			TrackBegin(TrackID);

			u32 Marker = ReadU32();
			u32 Len = ReadU32();

			u32 OrigEnd = DataLen;
			DataLen = DataPos + Len;

			//.. new track
			while (!ReadEof()) {
				u32 Delta = ReadLen();
				u8  Byte1 = ReadU8();
				u8  Msg = Byte1 & 0xF0;
				u8  Chanel = Byte1 & 0x0F;
				switch (Msg) {
				case 0x80:
				{ //  Note off
					u8  NoteNum = ReadU8();
					u8  NoteVelocity = ReadU8();
					NoteOff(Delta, NoteNum, NoteVelocity);
					break;
				}
				case 0x90:
				{ //  Note on 
					u8  NoteNum = ReadU8();
					u8  NoteVelocity = ReadU8();
					NoteOn(Delta, NoteNum, NoteVelocity);
					break;
				}
				case 0xA0:
				{ //  Polyphonic Key Pressure
					u8  NoteNum = ReadU8();
					u8  AmountOfpressure = ReadU8();
					PolyphonicKeyPressure(Delta, NoteNum, AmountOfpressure);
					break;
				}
				case 0xB0:
				{ //  Control Change 
					u8  ControllerNumber = ReadU8();
					u8  ControllerValue = ReadU8();
					ControlChange(Delta, ControllerNumber, ControllerValue);
					break;
				}
				case 0xC0:
				{ //  Program Change
					u8   ProgramNumber = ReadU8();
					ProgramChange(Delta, ProgramNumber);
					break;
				}
				case 0xD0:
				{ //  Channel Pressure 
					u8   PressureValue = ReadU8();
					ChannelPressure(Delta, PressureValue);
					break;
				}
				case 0xE0:
				{ //  Pitch Bend
					u8  MSB = ReadU8();
					u8  LSB = ReadU8();
					PitchBend(Delta, MSB, LSB);
					break;
				}
				case 0xF0:
				{ // Meta Event  
					MetaEventDelta(Delta);

					u8  Byte2 = ReadU8();
					u32 EventLen = ReadLen();
					switch (Byte2) {
					case 0x58:
					{//Time Signature 
						u8  nn = ReadU8();
						u8  dd = ReadU8();
						u8  cc = ReadU8();
						u8  bb = ReadU8();
						TimeSignature(nn, dd, cc, bb);
						break;
					}

					case 0x51:
					{//Set Tempo
						u32 Tempo = ReadU24();
						SetTempo(Tempo, round(60000000.0 / (f32)Tempo*1000.0)*0.001, ((f32)Tempo / (f32)PPQ / 1000000.0));
						break;
					}

					case 0x03:
					{// len text Sequence/Track Name
						char name[256];
						ReadStr((u8*)&name[0], EventLen);
						TrackName(name);
						break;
					}

					case 0x7F:
					{// Specific Meta-Event
						u32 SpecificLen = ReadLen();
						for (u32 n = 0; n < EventLen; n++) ReadU8();
						//..
						break;
					}

					case 0x2F:
					{//End of Track
						for (u32 n = 0; n < EventLen; n++) ReadU8();
						break;
					}

					default:  // Unk message ignored
						for (u32 n = 0; n < EventLen; n++) ReadU8();
						break;
					}
					break;
				}
				}
			}
			DataLen = OrigEnd;

			TrackEnd();
		}

		End();
	}

	virtual void Begin(u16 Tracks, u16 ticks_per_quarter) { //  Pulses Per Quarter  PPQ
	}
	virtual void End() {
	}
	virtual void TrackBegin(u32 Number) {
	}
	virtual void TrackEnd() {
	}
	virtual void SetTempo(u32 Tempo, f32 BPM, f32 seconds_per_tick) { //seconds = ticks * seconds_per_tick;
	}
	virtual void TimeSignature(u8 nn, u8 dd, u8 cc, u8 bb) {
	}
	virtual void TrackName(PConstStr Name) {
	}
	virtual void NoteOn(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
	}
	virtual void NoteOff(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
	}
	virtual void PolyphonicKeyPressure(u32 Delta, u8 NoteNum, u8 AmountOfpressure) {
	}
	virtual void ControlChange(u32 Delta, u8 ControllerNumber, u8 ControllerValue) {
	}
	virtual void ProgramChange(u32 Delta, u8 ProgramNumber) {
	}
	virtual void ChannelPressure(u32 Delta, u8 PressureValue) {
	}
	virtual void PitchBend(u32 Delta, u8 MSB, u8 LSB) {
	}
	virtual void MetaEventDelta(u32 Delta) {
	}

};


class MidiCalulateLen : public  MidiParser {
private:
	u64     CurrentTick;
	f32     CurrentSamples;
	f32     CurrentSecondsPerTick;
	f32     Samplerate;
public:
	u64     MidiTicks;
	f32     Samples;

	void    Init(u32 SampleRate) {
		MidiTicks = 0;
		Samples = 0;
		Samplerate = SampleRate;
		CurrentTick = 0;
	}

	virtual void Begin(u16 Tracks, u16 ticks_per_quarter) {
	}

	virtual void End() {
	}

	virtual void TrackBegin(u32 Number) {
		CurrentTick = 0;
		CurrentSamples = 0;
	}

	virtual void TrackEnd() {
		MidiTicks = max(MidiTicks, CurrentTick);
		Samples = max(Samples, CurrentSamples);
	}

	virtual void SetTempo(u32 Tempo, f32 BPM, f32 seconds_per_tick) {
		CurrentSecondsPerTick = seconds_per_tick;
	}

	virtual void TimeSignature(u8 nn, u8 dd, u8 cc, u8 bb) {
	}

	virtual void TrackName(PConstStr Name) {
	}

	virtual void NoteOn(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void NoteOff(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void PolyphonicKeyPressure(u32 Delta, u8 NoteNum, u8 AmountOfpressure) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ControlChange(u32 Delta, u8 ControllerNumber, u8 ControllerValue) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ProgramChange(u32 Delta, u8 ProgramNumber) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ChannelPressure(u32 Delta, u8 PressureValue) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void PitchBend(u32 Delta, u8 MSB, u8 LSB) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void MetaEventDelta(u32 Delta) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}
};

class MidiRender : public  MidiParser {
private:
	u64     CurrentTick;
	f32     CurrentSamples;
	f32     CurrentSecondsPerTick;
public:
	u32     Samplerate;
	u8      DataChanels = 0;
	f32*    Data[8];
	u64     DataLen;

	void    Render(u8 Chanels, u32 SampleRate, void* MidiFileData, u32 MidiFileLen) {
		DataChanels = Chanels;
		Samplerate = SampleRate;
		CurrentTick = 0;
		CurrentSamples = 0;

		MidiCalulateLen ParserLen;
		ParserLen.Init(SampleRate);
		ParserLen.Parse(MidiFileData, MidiFileLen);

		DataLen = ParserLen.Samples * SampleRate + 1;
		for (u32 i = 0; i < Chanels; i++) Data[i] = (f32*)HeapAlloc(GetProcessHeap(), 0, sizeof(f32) * DataLen);

		Parse(MidiFileData, MidiFileLen);
	}

	virtual void Begin(u16 Tracks, u16 ticks_per_quarter) {
	}

	virtual void End() {
	}

	virtual void TrackBegin(u32 Number) {
		CurrentTick = 0;
		CurrentSamples = 0;
	}

	virtual void TrackEnd() {

	}

	virtual void SetTempo(u32 Tempo, f32 BPM, f32 seconds_per_tick) {
		CurrentSecondsPerTick = seconds_per_tick;
	}

	virtual void TimeSignature(u8 nn, u8 dd, u8 cc, u8 bb) {
	}

	virtual void TrackName(PConstStr Name)
	{

	}

	virtual void NoteOn(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void NoteOff(u32 Delta, u8 NoteNum, u8 NoteVelocity) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void PolyphonicKeyPressure(u32 Delta, u8 NoteNum, u8 AmountOfpressure) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ControlChange(u32 Delta, u8 ControllerNumber, u8 ControllerValue) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ProgramChange(u32 Delta, u8 ProgramNumber) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void ChannelPressure(u32 Delta, u8 PressureValue) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void PitchBend(u32 Delta, u8 MSB, u8 LSB) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}

	virtual void MetaEventDelta(u32 Delta) {
		CurrentTick += Delta;
		CurrentSamples += Delta * CurrentSecondsPerTick;
	}
};

class CNoteManager
{
private:
	MidiCalulateLen midiParser;
	WAVE_FMT fmt;

public:
	void Initialize(WAVE_FMT WaveFormat);
	void Destroy();

	void GetCurrentVoiceState(size_t Synth, size_t Voice, NOTE_STRUCT& Struct);
};

