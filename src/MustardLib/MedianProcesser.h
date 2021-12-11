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
/// 输出结构体
/// </summary>
typedef struct OutputFrame {
	/// <summary>
	/// 中心频率
	/// </summary>
	long long	CenterFreq;

	/// <summary>
	/// 带宽
	/// </summary>
	int			Bandwidth;

	/// <summary>
	/// 信噪比
	/// </summary>
	float		SNR;

	/// <summary>
	/// 幅度
	/// </summary>
	float		AMP;
};


/// <summary>
/// 门限处理实体
/// </summary>
typedef struct ThresholdHandleEntity {
	/// <summary>
	/// 数据长度.
	/// </summary>
	int SampleCount = 0;

	/// <summary>
	/// 原始数据.
	/// </summary>
	short* OrigionData = nullptr;

	/// <summary>
	/// 门限帧长度.
	/// </summary>
	int MedianFilteringFrameLength = 0;

	/// <summary>
	/// 处理分辨率.
	/// </summary>
	float resolution = 0;

	/// <summary>
	/// 起始频率.
	/// </summary>
	INT64 startFrequency = 0;

	/// <summary>
	/// 滤波数据.
	/// </summary>
	short* MedianFilteringData = nullptr;

};


/* ================ 外部结构 ===================*/

/// <summary>
/// 输入帧结构.
/// </summary>
typedef struct InputFrame {
	/* Part 1 ---------------------*/

	/// <summary>
	/// 数据长度.
	/// </summary>
	int				SampleCount;

	/// <summary>
	/// 门限处理滑窗长度.
	/// </summary>
	int				HandleDataLength;

	/* Part 2 ---------------------*/

	/// <summary>
	/// 起始频率.(Hz)
	/// </summary>
	INT64			StartFrequency;

	/// <summary>
	/// 采点步长.
	/// </summary>
	int				FootLength;

	/// <summary>
	/// 处理分辨率.(Hz)
	/// </summary>
	float			Resolution;

	/// <summary>
	/// 限制门限.(dB)
	/// </summary>
	float			DetectLimitSNR;

	/* Part 3 ---------------------*/

	/// <summary>
	/// 数据帧数据.
	/// </summary>
	short* Datas;
};

/// <summary>
/// 输入帧结构.
/// </summary>
typedef struct IQInputFrame {
	/* Part 1 ---------------------*/

	/// <summary>
	/// 样点数.
	/// </summary>
	int				SampleCount;

	/// <summary>
	/// 门限处理滑窗长度.
	/// </summary>
	int				HandleDataLength;

	/* Part 2 ---------------------*/

	/// <summary>
	/// 中心频率.(Hz)
	/// </summary>
	INT64			CenterFrequency;

	/// <summary>
	/// 采样率.(Hz)
	/// </summary>
	INT64			SampleRate;

	/// <summary>
	/// 有效带宽
	/// </summary>
	INT64			ValidBandWidth;

	/// <summary>
	/// 时间戳.
	/// </summary>
	INT64			DataTimeStamp;

	/// <summary>
	/// 限制门限.(dB)
	/// </summary>
	float			DetectLimitSNR;

	/* Part 3 ---------------------*/

	/// <summary>
	/// 数据帧数据.
	/// </summary>
	short* Datas;
};

/// <summary>
/// 检测结果.
/// </summary>
typedef struct DetectResult {

	/// <summary>
	/// 检测结果中心频率.(Hz)
	/// </summary>
	INT64		CenterFreq;

	/// <summary>
	/// 检测结果带宽.(Hz)
	/// </summary>
	INT64		Bandwidth;

	/// <summary>
	/// 检测结果幅度.(dBm)
	/// </summary>
	float		Amp;

	/// <summary>
	/// 检测结果信噪比（误）.
	/// </summary>
	float		SNR;
};

/// <summary>
/// 检测结果一次性回调
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
	/// 当前数值地址
	/// </summary>
	int _peakPosition = 0;
	int _movePosition = 0;

	/// <summary>
	/// 数据缓冲数组
	/// </summary>
	short* _dataCache[10] = {};
	double* _averageCache = nullptr;

	ThresholdHandleEntity* _entity = nullptr;

private:
	void PeakOutSignal();
	// 返回计算出的门限
	bool Compute(bool isOutputDetectResult, short*);
	// 临时数据
	Ipp8u* _handleBuffer = nullptr;
	void* _userData = nullptr;


public:
	// FFT句柄。
	void* fftOnRealTimeHandle = nullptr;
	InputFrame* inputFrame = nullptr;
	DetectResultAllBack* Callback = nullptr;

public:
	MedianProcesser(void*);
	~MedianProcesser();

	/// <summary>
	/// 输入数据帧.
	/// </summary>
	/// <param name="frame">数据帧</param>
	bool InputData(InputFrame* frame, short*, bool isOutputDetectResult = true);
};

