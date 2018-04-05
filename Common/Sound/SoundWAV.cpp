#include <Common/Sound/Sound.h>
#include <System/File.h>

namespace Columbus
{

	typedef struct 
	{
		uint8 RIFFMagic[4]; //Magic bytes "RIFF"
		uint32 Size;        //Size of whole file (In WAVE file it Size - 8)
		uint8 WAVEMagic[4]; //Magic bytes "WAVE"
		uint8 FMTMagic[4];  //Magic bytes "fmt "
		uint32 SChunkSize;  //Must be 16
		uint16 Format;      //1 if PCM
		uint16 Channels;    //1 for mono, 2 for stereo
		uint32 Frequency;   //Sound fequency (22050Hz, 44100Hz, etc...)
		uint32 ByteRate;    //Bytes per second
		uint16 BlockAlign;  //Bytes per sample including all channels
		uint16 BPS;         //Bits per sample (8, 16, etc...)
		uint8 DataMagic[4]; //Magic bytes "data"
		uint32 DataSize;    //Size of sound data
	} WAV_HEADER;

	static bool ReadHeader(WAV_HEADER* Header, File* InFile)
	{
		if (Header == nullptr || InFile == nullptr) return false;

		if (!InFile->readBytes(Header->RIFFMagic, sizeof(Header->RIFFMagic))) return false;
		if (!InFile->readUint32(&Header->Size)) return false;
		if (!InFile->readBytes(Header->WAVEMagic, sizeof(Header->WAVEMagic))) return false;
		if (!InFile->readBytes(Header->FMTMagic, sizeof(Header->FMTMagic))) return false;
		if (!InFile->readUint32(&Header->SChunkSize)) return false;
		if (!InFile->readUint16(&Header->Format)) return false;
		if (!InFile->readUint16(&Header->Channels)) return false;
		if (!InFile->readUint32(&Header->Frequency)) return false;
		if (!InFile->readUint32(&Header->ByteRate)) return false;
		if (!InFile->readUint16(&Header->BlockAlign)) return false;
		if (!InFile->readUint16(&Header->BPS)) return false;
		if (!InFile->readBytes(Header->DataMagic, sizeof(Header->DataMagic))) return false;
		if (!InFile->readUint32(&Header->DataSize)) return false;

		return true;
	}

	bool SoundIsWAV(std::string FileName)
	{
		File WAVSoundFile(FileName, "rb");
		if (!WAVSoundFile.isOpened()) return false;

		uint8 Magic[12];
		WAVSoundFile.read(Magic, sizeof(Magic), 1);
		WAVSoundFile.close();

		if (Magic[0] == 'R' &&
		    Magic[1] == 'I' &&
		    Magic[2] == 'F' &&
		    Magic[3] == 'F')
		{
			//This file format is RIFF
			if (Magic[8] == 'W' &&
			    Magic[9] == 'A' &&
			    Magic[10] == 'V' &&
			    Magic[11] == 'E')
			{
				//This file format is WAVE
				return true;
			}
		}

		return false;
	}

	uint16* SoundLoadWAV(std::string FileName, uint64& OutSize, uint32& OutFrequency, uint16& OutChannels)
	{
		File WAVSoundFile(FileName, "rb");
		if (!WAVSoundFile.isOpened())
		{
			return nullptr;
		}

		WAV_HEADER Header;

		if (!ReadHeader(&Header, &WAVSoundFile))
		{
			return nullptr;
		}

		uint16* WAVSoundData = new uint16[Header.DataSize / sizeof(uint16)];

		if (!WAVSoundFile.readBytes(WAVSoundData, Header.DataSize))
		{
			delete[] WAVSoundData;
			return nullptr;
		}

		OutSize = Header.DataSize;
		OutFrequency = Header.Frequency;
		OutChannels = Header.Channels;

		return WAVSoundData;
	}

}














