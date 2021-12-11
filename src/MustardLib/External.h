#pragma once


#ifdef MEDIANDETECTOR
#define dllexp __declspec(dllexport)
#define MeAPI(outputType, name, functionName) dllexp outputType name functionName

#include "MedianProcesser.h"
#else
#define MeAPI(outputType, name, functionName) outputType __stdcall name functionName
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
	long long		StartFrequency;

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
	long long		CenterFrequency;

	/// <summary>
	/// 采样率.(Hz)
	/// </summary>
	long long		SampleRate;

	/// <summary>
	/// 有效带宽
	/// </summary>
	long long		ValidBandWidth;

	/// <summary>
	/// 时间戳.
	/// </summary>
	long long		DataTimeStamp;

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
	long long	CenterFreq;

	/// <summary>
	/// 检测结果带宽.(Hz)
	/// </summary>
	long long	Bandwidth;

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
typedef void __stdcall DetectResultAllBack(OutputFrame*, int, void* userData);
#endif


//***********************************************/
// 常规接口
//***********************************************/
#ifdef __cplusplus
extern "C" {
#endif
	/// <summary>
	/// 创建检测实例。
	/// </summary>
	/// <param name="dcllbk">检测结果回调</param>
	/// <param name="userData">用户辅助参数</param>
	/// <returns>检测实例句柄指针</returns>
	MeAPI(void*, CreateHandle, (DetectResultAllBack* dcllbk, void* userData));

	/// <summary>
	/// 运行处理.
	/// </summary>
	/// <param name="handle">实例句柄.</param>
	/// <param name="frame">输入数据帧.</param>
	/// <returns>门限数据</returns>
	MeAPI(bool, Compute, (void* handle, InputFrame* frame));

	/// <summary>
	/// 运行处理IQ.
	/// </summary>
	/// <param name="handle">实例句柄.</param>
	/// <param name="frame">输入数据帧.</param>
	/// <returns>门限数据</returns>
	MeAPI(bool, ExComputeIQ, (void* handle, IQInputFrame* frame));

	/// <summary>
	/// 获取中值门限.
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="frame"></param>
	/// <returns></returns>
	MeAPI(short*, GetThresholdline, (void* handle, InputFrame* frame));

	/// <summary>
	/// 获取中值门限（输入：double）
	/// </summary>
	/// <param name="origionData">输入原始数据</param>
	/// <param name="dataOut">输出数据</param>
	/// <param name="dataLen">输出长度</param>
	/// <param name="windowLen">窗长</param>
	MeAPI(void, DoThresholdCalculate_64f, (double* origionData, double* dataOut, int dataLen, int windowLen));

	/// <summary>
	/// 获取中值门限.
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="frame"></param>
	/// <returns></returns>
	MeAPI(short*, GetThresholdlineByIQ, (void* handle, IQInputFrame* frame));

	/// <summary>
	/// 释放检测实例。
	/// </summary>
	/// <param name="handle">检测实例句柄指针</param>
	MeAPI(void, Dispose, (void* handle));
#ifdef __cplusplus
}
#endif