#pragma once
#include <basetsd.h>
#include "math.h"
#include "ipps.h"
#include <thread>
#include <omp.h>
#include <iostream>

#ifndef SWAP
#define SWAP(a,b) temp = a; a = b; b = temp;
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

using namespace std;

/// <summary>
/// ����ṹ��
/// </summary>
typedef struct OutputFrame {
	/// <summary>
	/// ����Ƶ��
	/// </summary>
	long long	CenterFreq;

	/// <summary>
	/// ����
	/// </summary>
	int			Bandwidth;

	/// <summary>
	/// �����
	/// </summary>
	float		SNR;

	/// <summary>
	/// ����
	/// </summary>
	float		AMP;
};


/// <summary>
/// ���޴���ʵ��
/// </summary>
typedef struct ThresholdHandleEntity {
	/// <summary>
	/// ���ݳ���.
	/// </summary>
	int SampleCount = 0;

	/// <summary>
	/// ԭʼ����.
	/// </summary>
	short* OrigionData = nullptr;

	/// <summary>
	/// ����֡����.
	/// </summary>
	int MedianFilteringFrameLength = 0;

	/// <summary>
	/// ����ֱ���.
	/// </summary>
	float resolution = 0;

	/// <summary>
	/// ��ʼƵ��.
	/// </summary>
	INT64 startFrequency = 0;

	/// <summary>
	/// �˲�����.
	/// </summary>
	short* MedianFilteringData = nullptr;

};


/* ================ �ⲿ�ṹ ===================*/

/// <summary>
/// ����֡�ṹ.
/// </summary>
typedef struct InputFrame {
	/* Part 1 ---------------------*/

	/// <summary>
	/// ���ݳ���.
	/// </summary>
	int				SampleCount;

	/// <summary>
	/// ���޴���������.
	/// </summary>
	int				HandleDataLength;

	/* Part 2 ---------------------*/

	/// <summary>
	/// ��ʼƵ��.(Hz)
	/// </summary>
	INT64			StartFrequency;

	/// <summary>
	/// �ɵ㲽��.
	/// </summary>
	int				FootLength;

	/// <summary>
	/// ����ֱ���.(Hz)
	/// </summary>
	float			Resolution;

	/// <summary>
	/// ��������.(dB)
	/// </summary>
	float			DetectLimitSNR;

	/* Part 3 ---------------------*/

	/// <summary>
	/// ����֡����.
	/// </summary>
	short* Datas;
};

/// <summary>
/// ����֡�ṹ.
/// </summary>
typedef struct IQInputFrame {
	/* Part 1 ---------------------*/

	/// <summary>
	/// ������.
	/// </summary>
	int				SampleCount;

	/// <summary>
	/// ���޴���������.
	/// </summary>
	int				HandleDataLength;

	/* Part 2 ---------------------*/

	/// <summary>
	/// ����Ƶ��.(Hz)
	/// </summary>
	INT64			CenterFrequency;

	/// <summary>
	/// ������.(Hz)
	/// </summary>
	INT64			SampleRate;

	/// <summary>
	/// ��Ч����
	/// </summary>
	INT64			ValidBandWidth;

	/// <summary>
	/// ʱ���.
	/// </summary>
	INT64			DataTimeStamp;

	/// <summary>
	/// ��������.(dB)
	/// </summary>
	float			DetectLimitSNR;

	/* Part 3 ---------------------*/

	/// <summary>
	/// ����֡����.
	/// </summary>
	short* Datas;
};

/// <summary>
/// �����.
/// </summary>
typedef struct DetectResult {

	/// <summary>
	/// ���������Ƶ��.(Hz)
	/// </summary>
	INT64		CenterFreq;

	/// <summary>
	/// ���������.(Hz)
	/// </summary>
	INT64		Bandwidth;

	/// <summary>
	/// ���������.(dBm)
	/// </summary>
	float		Amp;

	/// <summary>
	/// ���������ȣ���.
	/// </summary>
	float		SNR;
};

/// <summary>
/// �����һ���Իص�
/// </summary>
typedef void __stdcall DetectResultAllBack(OutputFrame*, int, void*);

class MedianProcesser
{
private:
	OutputFrame* _outputsVecs = nullptr;

private:
	float _snr = 0;

	int _footLength = 30;
	bool _counting = false;
	bool _onProcess = false;
	bool _allowProcess = true;
	int	 _signalCount = 0;
	int	 _maxSignalCount = 0;

	short* _cutDownData = nullptr;
	short* _lastOveredMedian = nullptr;
	short* _cutThrsh = nullptr;
	short* _tempArr = nullptr;

	/// <summary>
	/// ��ǰ��ֵ��ַ
	/// </summary>
	int _peakPosition = 0;
	int _movePosition = 0;

	/// <summary>
	/// ���ݻ�������
	/// </summary>
	short* _dataCache[10] = {};
	double* _averageCache = nullptr;

	ThresholdHandleEntity* _entity = nullptr;

private:
	void PeakOutSignal();
	// ���ؼ����������
	bool Compute(bool isOutputDetectResult, short*);
	// ��ʱ����
	Ipp8u* _handleBuffer = nullptr;
	void* _userData = nullptr;


public:
	// FFT�����
	void* fftOnRealTimeHandle = nullptr;
	InputFrame* inputFrame = nullptr;
	DetectResultAllBack* Callback = nullptr;

public:
	MedianProcesser(void*);
	~MedianProcesser();

	/// <summary>
	/// ��������֡.
	/// </summary>
	/// <param name="frame">����֡</param>
	bool InputData(InputFrame* frame, short*, bool isOutputDetectResult = true);
};

