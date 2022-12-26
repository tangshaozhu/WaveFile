// WaveFileDemo.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "WaveFile.h"
#include "Crossover.h"
#include "FirFilter.h"
#include "IirFilter.h"
#include "fdacoefs.h"
#include "iircoefs-lc50-44100.h"
#include <string>
#include <cstdio>

#define TEST_FILE_PATH	//"F:/Documents/MFC/WaveFileDemo/Debug/"
#define TEST_FILE_NAME	"output"
#define MAX_PATH          260
char TestPath[MAX_PATH];



int FormatTest(int argc, char* argv[])
{
	int i;

	for (i = 1; i < argc; ++i) {
		WaveFile wav;
		wav.ReadFile(argv[i]);
		// float
		sprintf_s(TestPath, TEST_FILE_PATH "float" "(%d).wav", i);
		if (WF_FAILURE == wav.WriteFile(DT_FLOAT, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
		// int16
		sprintf_s(TestPath, TEST_FILE_PATH "int16" "(%d).wav", i);
		if (WF_FAILURE == wav.WriteFile(DT_INT16, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
		// int24
		sprintf_s(TestPath, TEST_FILE_PATH "int24" "(%d).wav", i);
		if (WF_FAILURE == wav.WriteFile(DT_INT24, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
		// int32
		sprintf_s(TestPath, TEST_FILE_PATH "int32" "(%d).wav", i);
		if (WF_FAILURE == wav.WriteFile(DT_INT32, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
		// uint8
		sprintf_s(TestPath, TEST_FILE_PATH "uint8" "(%d).wav", i);
		if (WF_FAILURE == wav.WriteFile(DT_UINT8, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
	}
#ifdef _DEBUG
	printf("输入任意键继续\n");
	getchar();
#endif
	return 0;
}

int TrimTest(int argc, char* argv[])
{
	int i;
	float sec;
	printf("输入裁剪秒数：");
	scanf_s("%f", &sec);
	fflush(stdin);
	for (i = 1; i < argc; ++i) {
		WaveFile wav;
		wav.ReadFile(argv[i]);
		// float
		strcpy_s(TestPath, MAX_PATH, argv[i]);
		size_t len = strlen(TestPath);
		if (len < 4) {
			printf("Invalid path: %s\n", TestPath);
			continue;
		}
		if (strcmp(TestPath + len - 4, ".wav") != 0) {
			printf("Invalid extension: %s\n", TestPath);
			continue;
		}
		sprintf(TestPath + len - 4, "_trim[%gs].wav", sec);
		DWORD sr = wav.GetSampleRate();
		if (WF_FAILURE == wav.WriteFile(DT_AUTO, TestPath, (int)((float)sr * sec))) {
			printf("Write file failed! [%s]\n", TestPath);
		}
	}
#ifdef _DEBUG
	printf("输入任意键继续\n");
	getchar();
#endif
	return 0;
}

int WavClear(int argc, char* argv[])
{
	int i;

	for (i = 1; i < argc; ++i) {
		WaveFile wav;
		wav.ReadFile(argv[i]);
		// float
		strcpy_s(TestPath, MAX_PATH, argv[i]);
		size_t len = strlen(TestPath);
		if (len < 4) {
			printf("Invalid path: %s\n", TestPath);
			continue;
		}

		if (WF_FAILURE == wav.WriteFile(DT_AUTO, argv[i])) {
			printf("Write file failed! [%s]\n", argv[i]);
		}
	}
#ifdef _DEBUG
	printf("输入任意键继续\n");
	getchar();
#endif
	return 0;
}


#if 1
#define CO_TEST_FILE "E:/Audio/Temp/sweep"
#else
#define CO_TEST_FILE "E:/Audio/Temp/sine440"
#endif

#define CO_TEST_EXT	 ".wav"

int CrossoverTest(int argc, char* argv[])
{
	WaveFile wav;		
	wav.ReadFile(CO_TEST_FILE CO_TEST_EXT);
	Crossover crs(10.0, 0.6);
	crs.ProcessWav(wav);
	wav.WriteFile(DT_AUTO, CO_TEST_FILE "_out.wav");
	return 0;
}
//int _tmain(int argc, _TCHAR* argv[])


int FilterTest(int argc, char* argv[])
{
	WaveFile wav;
	wav.ReadFile(CO_TEST_FILE CO_TEST_EXT);
	
	FirFilter ft(B + 250, 251);
	ft.ProcessWav(wav);
	wav.WriteFile(DT_AUTO, CO_TEST_FILE "_out.wav");
	return 0;
}


int IIRTest(int argc, char* argv[])
{
	WaveFile wav;
	wav.ReadFile(CO_TEST_FILE CO_TEST_EXT);

	mult_biquad_state sec1 = { 
		gain_sec1,
		denominator_sec1[0], denominator_sec1[1], denominator_sec1[2],
		numerator_sec1[0], numerator_sec1[1], numerator_sec1[2],
	};


	mult_biquad_state sec2 = {
		gain_sec2,
		denominator_sec2[0], denominator_sec2[1], denominator_sec2[2],
		numerator_sec2[0], numerator_sec2[1], numerator_sec2[2],
	};

	IirFilter iir(sec1, sec2);
	iir.ProcessWav(wav);
	wav.WriteFile(DT_AUTO, CO_TEST_FILE "_out.wav");
	return 0;
}

int OneKeyLowCut(int argc, char* argv[])
{
	int i;

	mult_biquad_state sec1 = {
		gain_sec1,
		denominator_sec1[0], denominator_sec1[1], denominator_sec1[2],
		numerator_sec1[0], numerator_sec1[1], numerator_sec1[2],
	};


	mult_biquad_state sec2 = {
		gain_sec2,
		denominator_sec2[0], denominator_sec2[1], denominator_sec2[2],
		numerator_sec2[0], numerator_sec2[1], numerator_sec2[2],
	};

	IirFilter iir(sec1, sec2);

	for (i = 1; i < argc; ++i) {
		WaveFile wav;
		wav.ReadFile(argv[i]);
		// float
		strcpy_s(TestPath, MAX_PATH, argv[i]);
		size_t len = strlen(TestPath);
		if (len < 4) {
			printf("Invalid path: %s\n", TestPath);
			continue;
		}
		if (strcmp(TestPath + len - 4, ".wav") != 0) {
			printf("Invalid extension: %s\n", TestPath);
			continue;
		}
		sprintf(TestPath + len - 4, "_lowcut.wav");

		iir.ProcessWav(wav);

		if (WF_FAILURE == wav.WriteFile(DT_AUTO, TestPath)) {
			printf("Write file failed! [%s]\n", TestPath);
		}
	}
	return 0;
}

int ConvertChannels(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i) {
		WaveFile wav;
		wav.ReadFile(argv[i]);
		// float
 		strcpy_s(TestPath, MAX_PATH, argv[i]);
		size_t len = strlen(TestPath);
		if (len < 4) {
			printf("Invalid path: %s\n", TestPath);
			continue;
		}
		if (strcmp(TestPath + len - 4, ".wav") != 0) {
			printf("Invalid extension: %s\n", TestPath);
			continue;
		}

		if (wav.GetChannels() == 2) {
			sprintf(TestPath + len - 4, "_L.wav");
			WaveFile wavL(wav, CHANNEL_MONO_L);
			wavL.WriteFile(DT_AUTO, TestPath);
			sprintf(TestPath + len - 4, "_R.wav");
			WaveFile wavR(wav, CHANNEL_MONO_R);
			wavR.WriteFile(DT_AUTO, TestPath);
		}
	}
	return 0;
}

int ConvertAndTrim(int argc, char* argv[])
{
	char* outfile = nullptr;
	if (argc == 2) {
		outfile = argv[1];
	} else if (argc > 2) {
		outfile = argv[2];
	}

	WaveFile* pWav = new WaveFile();
	pWav->ReadFile(argv[1]);

	if (pWav->GetChannels() == 2) {
		WaveFile* pWavTemp = new WaveFile(*pWav, CHANNEL_MONO_L);
		delete pWav;
		pWav =  pWavTemp;
	}
	size_t datalen = pWav->GetLastNonZero();
	printf("%d / %d\n", datalen, pWav->GetChannelLen());
	pWav->Resize(datalen);
	pWav->WriteFile(DT_AUTO, outfile);
	delete pWav;
	pWav = nullptr;
	return 0;
}

int main(int argc, char* argv[])
{
	return ConvertAndTrim(argc, argv);
}
