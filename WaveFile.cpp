/***************************************************************************************************
*
*
* <FILE>				WaveFile.cpp
*
*
* <AUTHOR>				tangshizhong
*
*
* <DATE>				2022��1��27��
*
*
* <DESCRIPTION>
*
*
*************************************************************************************************/
#include "stdafx.h"
#include "WaveFile.h"

#ifdef _DEBUG
#define DBG_PRINT(charp, ...) printf(charp"\n", __VA_ARGS__)
#else
#define DBG_PRINT(charp, ...)
#endif

#define PRINT_TAG(dw)	printf("TAG: [%c%c%c%c]\n", ((char*)&dw)[0], ((char*)&dw)[1], ((char*)&dw)[2], ((char*)&dw)[3])
#define TRUNCATE(a)		do {if ((a) < -1.0){ a = -1.0; } else if ((a) > 1.0) { a = 1.0; } } while(0)

WaveFile::~WaveFile()
{
	if (pData != nullptr) {
		delete[] pData;
		pData = nullptr;	// forget
	}
}

/**************************************************************************************************
*
*
* FUNCTION:				WaveFile
*
*
* INPUT PARAMETERS:		uint16 _channels		������
* 						uint32 _sampleRate		������
* 						eDataType eDataType	��������ö��ֵ
*
*
* RETURN VALUE:
*
*
* DESCRIPTION:			WaveFile���캯���������������������ʡ��������ʹ���
*
*
************************************************************************************************/
WaveFile::WaveFile(uint16 _channels, uint32 _sampleRate, WORD _dataType)
{
	/* �������������1��2������Ϊ1������ */
	if (_channels != 1 && _channels != 2) {
		printf_s("Invalid channel nums, set to mono\n");
		_channels = 1;
	}
	switch (_dataType)
	{
	case DT_UINT8:
	case DT_INT16:
	case DT_INT24:
	case DT_INT32:
	case DT_FLOAT:
		datatype = _dataType;
	default:
		datatype = DT_INT16;	// int16 for default
		break;
	}
	sampleRate = _sampleRate;
	channels = _channels;
	dataSize = 0;
	pData = nullptr;
}


/* �������� */
WaveFile::WaveFile(const WaveFile& that)
{
	sampleRate = that.sampleRate;
	channels = that.channels;
	dataSize = that.dataSize;
	datatype = that.datatype;

	if (that.pData == nullptr) {
		pData = nullptr;
	}
	else {
		pData = new sample_t[dataSize / sizeof(sample_t)];
		memcpy_s(pData, dataSize, that.pData, dataSize);
	}
}


WaveFile::WaveFile(const WaveFile& that, uint16 channelSel)
{
	channels = channelSel == CHANNEL_STEREO ? 2 : 1;
	sampleRate = that.sampleRate;
	datatype = that.datatype;

	if (that.pData == nullptr) {
		pData = nullptr;
	}

	if (channels == that.channels) {
		dataSize = that.dataSize;

		pData = new sample_t[dataSize / sizeof(sample_t)];
		memcpy_s(pData, dataSize, that.pData, dataSize);		
	}
	else if (that.channels == 1) {	// that: mono, this: stereo
		dataSize = that.dataSize * channels; // channels must be 2

		uint32 lentotal = dataSize / sizeof(sample_t);
		pData = new sample_t[lentotal];
		stereo_t streamDst = (stereo_t)pData;
		for (uint32 i = 0; i < lentotal / 2; i++)
		{
			streamDst[i][0] = streamDst[i][1] = that.pData[i];
		}			
		
	}
	else { // that: stereo, this: mono
		dataSize = that.dataSize / 2; // channels must be 2

		uint32 lentotal = dataSize / sizeof(sample_t);
		pData = new sample_t[lentotal];
		stereo_t streamSrc = (stereo_t)that.pData;
		
		const uint16 sel = 1 - (channelSel & 1);	// channelSel ���λ1��ȡ������0������1
		for (uint32 i = 0; i < lentotal; i++)
		{
			pData[i] = streamSrc[i][sel];
		}
		
	}

}

/**************************************************************************************************
*
*
* FUNCTION:				ReadFile
*
*
* INPUT PARAMETERS:		const char* path �ļ�·��
*
*
* RETURN VALUE:			WF_SUCCESS �ɹ�
* 						WF_FAILURE ʧ��
*
*
* DESCRIPTION:			TODO����Windows��ʹ��Mapping Fileʵ�ָù���
*
*
************************************************************************************************/
uint32 WaveFile::ReadFile(const char* path)
{
	FILE* fp = fopen(path, "rb");
	uint32 ret = WF_SUCCESS;
	if (fp == nullptr) {
		printf("read file failed, path: %s\n", path);
		return WF_FAILURE;
	}
	DBG_PRINT("file opened: [%s]", path);

	size_t readlen;
	DWORD readtag;
	DWORD readsize;
	WavHeaderStd head;	// �ļ����

	/****** 1. riff chunk ******/
	/* "RIFF" */
	readlen = fread(&readtag, sizeof(ChunkId_t), 1, fp);
	if (readlen != 1 || readtag != STR_RIFF) {
		printf("readlen: [%d]\n", readlen);
		printf("file format error! \"RIFF\": [%d] but read [%d]\n", STR_RIFF, readtag);
		ret = WF_FAILURE;
		goto CLOSE_FILE;
	}
	/* RIFF size */
	// ������ȡriff size��д�ļ�ʱ��Ҫ���¼���
	fseek(fp, sizeof(RiffChunk().dwSize), SEEK_CUR);

	/* WAVE */
	readlen = fread(&readtag, sizeof(ChunkId_t), 1, fp);
	if (readlen != 1 || readtag != STR_WAVE) {
		printf("file format error! \"WAVE\" ");
		PRINT_TAG(readtag);
		printf("\n");
		ret = WF_FAILURE;
		goto CLOSE_FILE;
	}

	// Read Chunks
	while (1) {
		/* read tag */
		readlen = fread(&readtag, sizeof(DWORD), 1, fp);
		if (readlen != 1) {
			ret = WF_FAILURE;
			goto CLOSE_FILE;
		}
		// PRINT_TAG(readtag);
		switch (readtag)
		{
		case STR_FMT:	/* [fmt ] */
			/* read size */
			fread(&readsize, sizeof(DWORD), 1, fp);
			if (readsize < 16) {
				printf("fmt chuck is too short! format size: %u\n", readsize);
				ret = WF_FAILURE;
				goto CLOSE_FILE;
			}

			fread(&head.stFormatChunk.wFormatTag, 16, 1, fp);
			//PrintFormatChunk();
			fseek(fp, readsize - 16, SEEK_CUR);
			sampleRate = head.stFormatChunk.dwSamplesPerSec;
			channels = head.stFormatChunk.wChannels;
			if (head.stFormatChunk.wFormatTag == FMT_TAG_IEEE754_FLOAT) {
				datatype = DT_FLOAT;
			}
			else {
				datatype = head.stFormatChunk.usBitsPerSample / 8;
			}
			break;
		case STR_DATA:	/* [data] */
		{
			/* read size */
			DWORD fileDataSize;
			DWORD newDataSize;
			fread(&fileDataSize, sizeof(DWORD), 1, fp);
			head.stDataChunk.dwSize = fileDataSize;

			switch (datatype)
			{
			case DT_INT16:
			{
				size_t sampleNum = fileDataSize / DT_INT16;
				newDataSize = sampleNum * sizeof(sample_t);
				int16* pFileData = new int16[sampleNum];
				fread(pFileData, DT_INT16, sampleNum, fp);

				if (pData != nullptr) {
					// ���dataSize���������ļ����ݣ���Ҫ���������ڴ档����㹻����Ҫ��
					if (dataSize < newDataSize) {
						delete[] pData;
						pData = new sample_t[sampleNum];
					}
				} else {
					pData = new sample_t[sampleNum];
				}
				dataSize = newDataSize;

				for (DWORD i = 0; i < sampleNum; ++i) {
					pData[i] = ((sample_t)(pFileData[i])) / (sample_t)0x7fff;
				}
				delete[] pFileData;
				break;
			}
			case DT_INT32:
			{
				size_t sampleNum = fileDataSize / DT_INT32;
				newDataSize = sampleNum * sizeof(sample_t);
				int32* pFileData = new int32[sampleNum];
				fread(pFileData, DT_INT32, sampleNum, fp);

				if (pData != nullptr) {
					// ���dataSize���������ļ����ݣ���Ҫ���������ڴ档����㹻����Ҫ��
					if (dataSize < fileDataSize) {
						delete[] pData;
						pData = nullptr;	// forget
						pData = new sample_t[sampleNum];
					} 
				}
				else {
					pData = new sample_t[sampleNum];
				}
				dataSize = fileDataSize;

				for (DWORD i = 0; i < sampleNum; ++i) {
					pData[i] = (sample_t)(pFileData[i]) / (sample_t)0x7fffffff;
				}
				delete[] pFileData;
				break;
			}
			case DT_INT24:
			{
				size_t sampleNum = fileDataSize / DT_INT24;
				newDataSize = sampleNum * sizeof(sample_t);
				BYTE* pFileData = new BYTE[fileDataSize];
				fread(pFileData, 1, fileDataSize, fp);

				if (pData != nullptr) {
					// ���dataSize���������ļ����ݣ���Ҫ���������ڴ档����㹻����Ҫ��
					if (dataSize < newDataSize) {
						delete[] pData;
						pData = new sample_t[sampleNum];
					}
				}
				else {
					pData = new sample_t[sampleNum];
				}
				dataSize = newDataSize;

				for (DWORD i = 0, si = 0; si < sampleNum; i += DT_INT24, ++si) {
					int32 itemp = 0;
					((BYTE*)(&itemp))[1] = pFileData[i + 0];
					((BYTE*)(&itemp))[2] = pFileData[i + 1];
					((BYTE*)(&itemp))[3] = pFileData[i + 2];
					pData[si] = (sample_t)(itemp) / (sample_t)0x7fffffff;
				}
				delete[] pFileData;
				break;
			}
			case DT_FLOAT:
			{
				size_t sampleNum = fileDataSize / DT_INT32;
				newDataSize = sampleNum * sizeof(sample_t);
				float32* pFileData = new float32[sampleNum];
				fread(pFileData, DT_INT32, sampleNum, fp);

				if (pData != nullptr) {
					// ���dataSize���������ļ����ݣ���Ҫ���������ڴ档����㹻����Ҫ��
					if (dataSize < fileDataSize) {
						delete[] pData;
						pData = nullptr;	// forget
						pData = new sample_t[sampleNum];
					}
				}
				else {
					pData = new sample_t[sampleNum];
				}
				dataSize = fileDataSize;

				for (DWORD i = 0; i < sampleNum; ++i) {
					pData[i] = (sample_t)(pFileData[i]);
				}
				delete[] pFileData;
				break;
			}
			default:
				printf("WaveFile error data type! %u\n", datatype);
				datatype = DT_INT16;
				ret = WF_FAILURE;
				goto CLOSE_FILE;
				break;
			}
			goto CLOSE_FILE;
		}
		default:
			PRINT_TAG(readtag);
			printf("Other Chunk, read chunk size and discard the data.\n");
			fread(&readsize, sizeof(DWORD), 1, fp);
			fseek(fp, readsize, SEEK_CUR);
			break;
		}
	};


CLOSE_FILE:
	fclose(fp);
	return ret;
}


uint32 WaveFile::WriteFile(WORD _datatype, const char* path, DWORD start, int len)
{
	// �������ݴ�С
	DWORD datalen = dataSize / sizeof(sample_t);

	if (pData == nullptr || datalen <= start * channels || len <= 0) {
		printf("WaveFile::WriteFile() >>>> No data to write!\n");
		return WF_FAILURE;
	}
	DWORD leftlen = datalen - start * channels;
	DWORD lentotal = ((DWORD)len) * channels;
	lentotal = lentotal < leftlen ? lentotal : leftlen;

	// ���ļ�
	FILE* fp = fopen(path, "wb");	// write only
	uint32 ret = WF_SUCCESS;
	printf("file created: [%s]\n", path);
	if (fp == nullptr) {
		printf("create file failed, path: %s", path);
		return WF_FAILURE;
	}
	// �����ļ�ͷ
	WavHeaderStd head;
	uint16 bytesPerSample = GetSampleSize(_datatype);
	uint16 bitdepth = bytesPerSample * 8;
	DWORD fileDataSize = lentotal * bytesPerSample;

	// дriff size
	head.stRiffChunk.dwSize = sizeof(WavHeaderStd) - sizeof(head.stRiffChunk.c_ui_riff) - sizeof(head.stRiffChunk.dwSize) + fileDataSize;

	// дformat chunk
	head.stFormatChunk.dwSize = 16;
	head.stFormatChunk.wFormatTag = GetFormatTag(_datatype);
	head.stFormatChunk.wChannels = channels;
	head.stFormatChunk.dwSamplesPerSec = sampleRate;
	head.stFormatChunk.dwAvgBytesPerSec = channels * sampleRate * bytesPerSample;
	head.stFormatChunk.wblockAlign = channels * bytesPerSample;
	head.stFormatChunk.usBitsPerSample = bitdepth;

	// дdata size
	head.stDataChunk.dwSize = fileDataSize;

	fwrite(&head.stRiffChunk, sizeof(head.stRiffChunk), 1, fp);
	fwrite(&head.stFormatChunk, sizeof(head.stFormatChunk), 1, fp);
	fwrite(&head.stDataChunk, sizeof(head.stDataChunk), 1, fp);

	WriteDataToFile(fp, _datatype, start, len);

	fclose(fp);
	return ret;
}


// ��ӡ�ļ�ͷ��Ϣ
void WaveFile::PrintInfo(WavHeaderStd& head)
{
	printf("%u %u bit %s\n", sampleRate, head.stFormatChunk.usBitsPerSample, channels == 2 ? "������" : "������");
	PRINT_TAG(head.stRiffChunk.c_ui_riff);
	printf("riff size: %u\n", head.stRiffChunk.dwSize);
	PRINT_TAG(head.stRiffChunk.c_ui_wave);
	PRINT_TAG(head.stFormatChunk.ui_fmt);
	PrintFormatChunk(head);
	PRINT_TAG(head.stDataChunk.ui_data);
}


void WaveFile::PrintFormatChunk(WavHeaderStd& head)
{
	printf("fmt size: %u\n", head.stFormatChunk.dwSize);
	printf("wFormatTag: %u, wChannels: %u\n", head.stFormatChunk.wFormatTag, head.stFormatChunk.wChannels);
	printf("dwSamplesPerSec: %u\n", head.stFormatChunk.dwSamplesPerSec);
	printf("dwAvgBytesPerSec: %u\n", head.stFormatChunk.dwAvgBytesPerSec);
	printf("wblockAlign: %u, usBitsPerSample: %u\n", head.stFormatChunk.wblockAlign, head.stFormatChunk.usBitsPerSample);
}


WORD WaveFile::GetFormatTag(WORD _datatype)
{
	switch (_datatype)
	{
	case DT_UINT8:
	case DT_INT16:
	case DT_INT24:
	case DT_INT32:
		return FMT_TAG_MICROSOFT_PCM;
	case DT_FLOAT:
		return FMT_TAG_IEEE754_FLOAT;
		break;
	case DT_AUTO:
	default:
		return GetFormatTag(datatype);	// ����ⲿ������DT_AUTO�򲻿������ݣ�ʹ���ڲ��������� datatype
		break;
	}
}


WORD WaveFile::GetSampleSize(WORD _datatype)
{
	switch (_datatype)
	{
	case DT_UINT8:
	case DT_INT16:
	case DT_INT24:
	case DT_INT32:
		return _datatype;
	case DT_FLOAT:
		return sizeof(float32);
	case DT_AUTO:
	default:
		return GetSampleSize(datatype);	// ����ⲿ������DT_AUTO�򲻿������ݣ�ʹ���ڲ��������� datatype
	}
}


void WaveFile::WriteDataToFile(FILE* fpWrite, WORD _datatype, DWORD start, int len)
{
	DWORD datalen = dataSize / sizeof(sample_t);

	if (pData == nullptr || datalen <= start * channels || len <= 0) {
		printf("No data to write!\n");
		return;
	}
	DWORD leftlen = datalen - start * channels;
	DWORD lentotal = ((DWORD)len) * channels;
	lentotal = lentotal < leftlen ? lentotal : leftlen;
	sample_t* pDataOffset = pData + start * channels;

	switch (_datatype)
	{
	case DT_UINT8:
	{
		uint8* pDataWrite = new uint8[lentotal];

		for (DWORD i = 0; i < lentotal; ++i) {
			TRUNCATE(pDataOffset[i]);
			pDataWrite[i] = (int8)(pDataOffset[i] * (sample_t)0x7f);
			pDataWrite[i] ^= 0x80;	// to unsigned char
		}
		fwrite(pDataWrite, DT_UINT8, lentotal, fpWrite);
		delete[] pDataWrite;
		break;
	}
	case DT_INT24:
	{
		BYTE* pDataWrite = new BYTE[lentotal * DT_INT24];

		for (DWORD bi = 0, si = 0; si < lentotal; si += 1, bi += 3) {
			TRUNCATE(pDataOffset[si]);
			int32 itemp = (int32)(pDataOffset[si] * (sample_t)0x7fffffff);
			pDataWrite[bi + 0] = ((BYTE*)(&itemp))[1];
			pDataWrite[bi + 1] = ((BYTE*)(&itemp))[2];
			pDataWrite[bi + 2] = ((BYTE*)(&itemp))[3];
		}
		fwrite(pDataWrite, DT_INT24, lentotal, fpWrite);
		delete[] pDataWrite;
		break;
	}
	case DT_INT32:
	{
		int32* pDataWrite = new int32[lentotal];
		for (DWORD i = 0; i < lentotal; ++i) {
			pDataWrite[i] = (int32)(pDataOffset[i] * (sample_t)0x7fffffff);

		}
		fwrite(pDataWrite, DT_INT32, lentotal, fpWrite);
		delete[] pDataWrite;
		break;
	}
	case DT_FLOAT:
	{
		float32* pDataWrite = new float32[lentotal];
		for (DWORD i = 0; i < lentotal; ++i) {
			pDataWrite[i] = (float32)(pDataOffset[i]);

		}
		fwrite(pDataWrite, DT_INT32, lentotal, fpWrite);
		delete[] pDataWrite;
		break;
	}
	case DT_INT16:
	{
		int16* pDataWrite = new int16[lentotal];

		for (DWORD i = 0; i < lentotal; ++i) {
			TRUNCATE(pDataOffset[i]);
			pDataWrite[i] = (int16)(pDataOffset[i] * (sample_t)0x7fff);

		}
		fwrite(pDataWrite, DT_INT16, lentotal, fpWrite);
		delete[] pDataWrite;
		break;
	}
	case DT_AUTO:
	default:
		WriteDataToFile(fpWrite, datatype, start, len);	// ����ⲿ������DT_AUTO�򲻿������ݣ�ʹ���ڲ��������� datatype
		break;
	}
}


