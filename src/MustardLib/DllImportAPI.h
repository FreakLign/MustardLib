#pragma once
typedef short* shorts;
#define MDEF(type, name ,value) typedef type name value
typedef void* MDHANDLE;

// 输出结构体
typedef struct OutputFrame {
	long long	CenterFreq;		// 中心频率
	int			Bandwidth;		// 带宽
	float		SNR;			// 信噪比
	float		AMP;			// 幅度
}*POutputFrame;

// 输入帧结构（频谱数据）
typedef struct InputFrame {
	int				DataLength;				// 数据长度
	int				HandleDataLength;		// 门限处理滑窗长度
	long long		StartFrequency;			// 起始频率(Hz)
	int				FootLength;				// 采点步长
	float			Resolution;				// 处理分辨率
	float			DetectLimitSNR;			// 限制门限
	shorts			Datas;					// 数据帧数据
}*PInputFrame;

// 输入帧结构（IQ数据）
typedef struct IQInputFrame {
	int				DataLength;				// 样点数
	int				HandleDataLength;		// 门限处理滑窗长度
	long long 		CenterFrequency;		// 中心频率
	long long		SampleRate;				// 采样率
	long long		ValidBandWidth;			// 有效带宽
	long long		DataTimeStamp;			// 时间戳
	float			DetectLimitSNR;			// 限制门限
	shorts			Datas;					// 数据帧数据
}*PIQInputFrame;

// 检测结果回调
// Para 0	：回调结果指针
// Para 1	：回调结果数量
typedef void __stdcall DetectResultAllBack(POutputFrame, int), *PDetectResultAllBack;

// 创建检测处理实例
// callback	：检测结果回调函数指针
// 返回值	：创建实例句柄
MDEF(MDHANDLE, CreateHandle, (PDetectResultAllBack callback));

// 以频谱数据检测信号
// handle	：处理实例
// frame	：输入数据帧
// 返回值	：执行情况
MDEF(bool, Compute, (MDHANDLE handle, PInputFrame frame));

// 以IQ数据监测信号
// handle	：处理实例
// frame	：输入数据帧
// 返回值	：执行情况
MDEF(bool, ExComputeIQ, (MDHANDLE handle, PIQInputFrame frame));

// 释放实例
// handle	：处理实例
MDEF(void, Dispose, (MDHANDLE handle));