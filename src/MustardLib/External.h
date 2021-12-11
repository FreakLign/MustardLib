#pragma once


#ifdef MEDIANDETECTOR
#define dllexp __declspec(dllexport)
#define MeAPI(outputType, name, functionName) dllexp outputType name functionName

#include "MedianProcesser.h"
#else
#define MeAPI(outputType, name, functionName) outputType __stdcall name functionName
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
	long long		StartFrequency;

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
	long long		CenterFrequency;

	/// <summary>
	/// ������.(Hz)
	/// </summary>
	long long		SampleRate;

	/// <summary>
	/// ��Ч����
	/// </summary>
	long long		ValidBandWidth;

	/// <summary>
	/// ʱ���.
	/// </summary>
	long long		DataTimeStamp;

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
	long long	CenterFreq;

	/// <summary>
	/// ���������.(Hz)
	/// </summary>
	long long	Bandwidth;

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
typedef void __stdcall DetectResultAllBack(OutputFrame*, int, void* userData);
#endif


//***********************************************/
// ����ӿ�
//***********************************************/
#ifdef __cplusplus
extern "C" {
#endif
	/// <summary>
	/// �������ʵ����
	/// </summary>
	/// <param name="dcllbk">������ص�</param>
	/// <param name="userData">�û���������</param>
	/// <returns>���ʵ�����ָ��</returns>
	MeAPI(void*, CreateHandle, (DetectResultAllBack* dcllbk, void* userData));

	/// <summary>
	/// ���д���.
	/// </summary>
	/// <param name="handle">ʵ�����.</param>
	/// <param name="frame">��������֡.</param>
	/// <returns>��������</returns>
	MeAPI(bool, Compute, (void* handle, InputFrame* frame));

	/// <summary>
	/// ���д���IQ.
	/// </summary>
	/// <param name="handle">ʵ�����.</param>
	/// <param name="frame">��������֡.</param>
	/// <returns>��������</returns>
	MeAPI(bool, ExComputeIQ, (void* handle, IQInputFrame* frame));

	/// <summary>
	/// ��ȡ��ֵ����.
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="frame"></param>
	/// <returns></returns>
	MeAPI(short*, GetThresholdline, (void* handle, InputFrame* frame));

	/// <summary>
	/// ��ȡ��ֵ���ޣ����룺double��
	/// </summary>
	/// <param name="origionData">����ԭʼ����</param>
	/// <param name="dataOut">�������</param>
	/// <param name="dataLen">�������</param>
	/// <param name="windowLen">����</param>
	MeAPI(void, DoThresholdCalculate_64f, (double* origionData, double* dataOut, int dataLen, int windowLen));

	/// <summary>
	/// ��ȡ��ֵ����.
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="frame"></param>
	/// <returns></returns>
	MeAPI(short*, GetThresholdlineByIQ, (void* handle, IQInputFrame* frame));

	/// <summary>
	/// �ͷż��ʵ����
	/// </summary>
	/// <param name="handle">���ʵ�����ָ��</param>
	MeAPI(void, Dispose, (void* handle));
#ifdef __cplusplus
}
#endif