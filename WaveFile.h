/***************************************************************************************************
*
*
* <FILE>				WaveFile.h
*
*
* <AUTHOR>				tangshizhong
*
*
* <DATE>				2022年1月27日
*
*
* <DESCRIPTION>			Wave文件类
* 						参照业界习惯，处理Wave文件数据时一律采用float32/64，保证编辑过程不失真。
*
*
*
*************************************************************************************************/

#pragma once
//#include <windows.h>
#include "wav-types.h"

#define	WF_SUCCESS 0
#define	WF_FAILURE -1
/* Chunk string definitions */
#define STR_RIFF		0x46464952	// "RIFF": RIFF chunk
#define STR_WAVE		0x45564157	// "WAVE": wave chunk
#define STR_FMT			0x20746d66	// "fmt ": format chunk
#define STR_DATA		0x61746164	// "data": data chunk

// useless chunks
#define STR_JUNK		0x4b4e554a	// "JUNK": junk chunk
#define STR_BEXT		0x74786562	// "bext": bext chunk
#define STR_FAKE		0x656b6146	// "Fake": Fake chunk
#define STR_ACID		0x64696361	// "acid": acid chunk

// Audio format: https://blog.csdn.net/yi7900/article/details/7481599
#define FMT_TAG_MICROSOFT_PCM        1
#define FMT_TAG_MICROSOFT_ADPCM      2
#define FMT_TAG_IEEE754_FLOAT        3
#define FMT_TAG_MICROSOFT_ALAW       6
#define FMT_TAG_MICROSOFT_MULAW      7
#define FMT_TAG_MPEG                 80
#define FMT_TAG_MP3                  85


typedef float64(*stereo_f64_t)[2];
typedef float64* mono_f64_t;
typedef float32(*stereo_f32_t)[2];
typedef float32* mono_f32_t;
typedef int16(*stereo_i16_t)[2];
typedef int16* mono_i16_t;
typedef int32(*stereo_i32_t)[2];
typedef int32* mono_i32_t;
typedef int8(*stereo_i8_t)[2];
typedef int8* mono_i8_t;
typedef uint ChunkId_t;

struct RiffChunk
{
	const ChunkId_t c_ui_riff = STR_RIFF;    // RIFF Header   Magic header
	DWORD dwSize;   // RIFF Chunk Size
	const ChunkId_t c_ui_wave = STR_WAVE;    // WAVE Header
}; /* 12 bytes */

struct FormatChunk
{
	const ChunkId_t ui_fmt = STR_FMT;     // FMT header
	DWORD dwSize; // Size of the fmt chunk, generally 16
	WORD wFormatTag; // Audio format 1=PCM, 6=alaw,7=mulaw, 
	WORD wChannels;   // Number of channels 1 = Mono 2 = Stereo
	DWORD dwSamplesPerSec; // Sampling Frequency in Hz
	DWORD dwAvgBytesPerSec;  // bytes per second
	WORD wblockAlign;   // 2=16-bit mono, 4=16-bit stereo, wChannels * usBitsPerSample * dwSamplesPerSec
	USHORT usBitsPerSample; // Number of bits per sample, bitdepth
}; /* 24 bytes */

struct DataChunk
{
	const unsigned int ui_data = STR_DATA; // "data" string
	DWORD dwSize; // Sampled data length
};  /* 8 bytes */

struct WavHeaderStd
{
	RiffChunk stRiffChunk;
	FormatChunk stFormatChunk;
	DataChunk stDataChunk;
}; /* 44 bytes */



#define EM_UINT8 1
#define	EM_INT16 2
#define	EM_INT24 3
#define	EM_INT32 4
#define	EM_FLOAT 7
#define	EM_AUTO  8

class WaveFile
{
public:
	friend class FirFilter;
	friend class IirFilter;
	friend class Disorter;
	// WaveFile();                      // 默认构造
	WaveFile(const WaveFile& that);     // 拷贝构造
	WaveFile(uint16 _channels = 2, uint32 _sampleRate = 44100, WORD dataType = EM_INT16);    // 构造函数，新建
	~WaveFile();
	
	uint32 ReadFile(const char* path);	/* 读取文件 */

	void Resize(uint32 size);
	uint32 WriteFile(WORD _datatype, const char* path, DWORD start = 0, int len = 0x7fffffff);


	/* Gets Funcs */
	uint32 GetDataSize() const { return dataSize; }
	uint16 GetChannels() const { return channels; }
	uint32 GetSampleRate(void) const { return sampleRate; }
	uint32 GetFormat() const;


	size_t GetChannelLen(void) const
	{
		return dataSize / channels / sizeof(float32);
	}

	void ImportData(size_t size, const float32* pSrc);
	void ImportData(size_t chSize, const float32* pSrcL, const float32* pSrcR);
	void ExportData(size_t size, float32* pDst);
	void ExportData(size_t chSize, float32* pDstL, float32* pDstR);

	/* 接管WavHeaderStd方法 */
	virtual void PrintInfo(WavHeaderStd& head);		// 打印文件头信息
	virtual void PrintFormatChunk(WavHeaderStd& head);

	friend void Convolution(DWORD start, DWORD len, const float ir);	// 友元函数：卷积

protected:
	uint16 datatype;
	uint32 sampleRate;
	uint16 channels;
	uint32 dataSize;    // 真实数据大小，以float32类型数据计
	float32 *pData;
	WORD GetFormatTag(WORD _datatype);
	WORD GetSampleSize(WORD _datatype);
	void WriteDataToFile(FILE* fpWrite, uint16 _datatype, DWORD start, int len);

};

