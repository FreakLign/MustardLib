#pragma once
#include "External.h"
#include "DFT.h"
#include <basetsd.h>

extern "C" {
	short* thr_line = nullptr;
	int thr_line_len = 0;
#pragma region FFT
	int fftLength = 0;
	void* fftHandle = nullptr;
	float* fftData = nullptr;
	short* fftResult = nullptr;
#pragma endregion

	void* CreateHandle(DetectResultAllBack* dcllbk, void* userData)
	{
		MedianProcesser* handle = new MedianProcesser(userData);
		handle->Callback = dcllbk;
		return handle;
	}

	bool Compute(void* handle, InputFrame* frame)
	{
		if (!(MedianProcesser*)handle)return false;
		return (((MedianProcesser*)handle)->InputData(frame, nullptr));
	}

	bool ExComputeIQ(void* handle, IQInputFrame* frame)
	{
		if (!(MedianProcesser*)handle)return false;
		if (((MedianProcesser*)handle)->fftOnRealTimeHandle == nullptr) {
			((MedianProcesser*)handle)->fftOnRealTimeHandle = new CDFT();
			((CDFT*)((MedianProcesser*)handle)->fftOnRealTimeHandle)->Init(frame->SampleCount, WinNone, 0, true);
		}
		if (((MedianProcesser*)handle)->inputFrame == nullptr) {
			((MedianProcesser*)handle)->inputFrame = new InputFrame();
			((MedianProcesser*)handle)->inputFrame->Datas = new short[frame->SampleCount];
		}
		auto ths_data = new float[frame->SampleCount];
		((CDFT*)((MedianProcesser*)handle)->fftOnRealTimeHandle)->DFT((sc16*)frame->Datas, nullptr, nullptr, ths_data, 0);
		ippsConvert_32f16s_Sfs(ths_data, ((MedianProcesser*)handle)->inputFrame->Datas, frame->SampleCount, ippRndNear, 0);
		delete[] ths_data;
		((MedianProcesser*)handle)->inputFrame->DetectLimitSNR = frame->DetectLimitSNR;
		((MedianProcesser*)handle)->inputFrame->HandleDataLength = frame->HandleDataLength;
		((MedianProcesser*)handle)->inputFrame->Resolution = (float)(frame->SampleRate) / (float)(frame->SampleCount);
		((MedianProcesser*)handle)->inputFrame->StartFrequency = frame->CenterFrequency - frame->ValidBandWidth / 2;
		((MedianProcesser*)handle)->inputFrame->FootLength = 1;
		int overLength = (frame->SampleRate - frame->ValidBandWidth) / ((MedianProcesser*)handle)->inputFrame->Resolution;
		((MedianProcesser*)handle)->inputFrame->SampleCount = frame->SampleCount - overLength;
		ippsCopy_16s(((MedianProcesser*)handle)->inputFrame->Datas + overLength / 2, ((MedianProcesser*)handle)->inputFrame->Datas, ((MedianProcesser*)handle)->inputFrame->SampleCount);
		return (((MedianProcesser*)handle)->InputData(((MedianProcesser*)handle)->inputFrame, nullptr));
	}

	short* GetThresholdline(void* handle, InputFrame* frame)
	{
		if (!(MedianProcesser*)handle)return nullptr;
		if (thr_line == nullptr) {
			if (thr_line_len != frame->SampleCount) {
				thr_line_len = frame->SampleCount;
				thr_line = new short[frame->SampleCount];
			}
		}
		else if (thr_line_len != frame->SampleCount) {
			delete[] thr_line;
			thr_line_len = frame->SampleCount;
			thr_line = new short[frame->SampleCount];
		}
		if ((((MedianProcesser*)handle)->InputData(frame, thr_line, false)));
		return thr_line;
	}

	void DoThresholdCalculate_64f(double* origionData, double* dataOut, int dataLen, int windowLen)
	{
		double sum = 0;
		size_t sumTime = 0;
		double maxValue = 0;
		double minValue = 0;
		double lastAddValue = 0;
		double shiftValue = 0;
		Ipp8u* buf;
		int bufSize;
		ippsFilterMedianGetBufferSize(windowLen, ipp8u, &bufSize);
		buf = ippsMalloc_8u(bufSize);
		ippsFilterMedian_64f(origionData, dataOut, dataLen, windowLen, 0, 0, buf);
		ippsFree(buf);
		// 简述：
		//		自适应门限计算。
		// 详情：
		//		取1和原始数据与中值的差值之间的最大值作为初始点，后续做原始值与中值的差值，若
		//		差值大于前序值的平均值（sum / sumTime），则计算其与前序平均值的比值的两倍作为
		//		放大权值，若差值小于前序值的平均值并大于0，若计算其与前序平均值的比值的两倍作
		//		为收束权值。
		for (INT64 i = windowLen / 2 + (INT64)1; i < (INT64)dataLen - windowLen / 2 - (INT64)1; i++) {
			if (sumTime == 0) lastAddValue = origionData[i] - dataOut[i] > 1 ? origionData[i] - dataOut[i] : 1;
			else {
				if (origionData[i] - dataOut[i] > (sum / sumTime)) {
					lastAddValue = ((origionData[i] - dataOut[i]) / (sum / sumTime)) * sum * 2.0 / sumTime;
				}
				else {
					if (origionData[i] - dataOut[i] > 0) {
						lastAddValue -= ((origionData[i] - dataOut[i]) / (sum / sumTime)) * sum / 2.0 / sumTime;
					}
					else {
						lastAddValue = sum / sumTime;
					}
				}
			}
			sum += lastAddValue;
			sumTime++;
		}
		shiftValue = 2.0 * sum / sumTime;
		ippsAddC_64f_I(shiftValue, dataOut + windowLen / 2 + 1, dataLen - windowLen / 2 - 1);
		ippsMove_64f(dataOut + windowLen / 2, dataOut, dataLen - windowLen / 2);
	}

	short* GetThresholdlineByIQ(void* handle, IQInputFrame* frame)
	{
		if (!(MedianProcesser*)handle)return false;
		if (((MedianProcesser*)handle)->fftOnRealTimeHandle == nullptr) {
			((MedianProcesser*)handle)->fftOnRealTimeHandle = new CDFT();
			((CDFT*)((MedianProcesser*)handle)->fftOnRealTimeHandle)->Init(frame->SampleCount, WinNone, 0, true);
		}
		if (((MedianProcesser*)handle)->inputFrame == nullptr) {
			((MedianProcesser*)handle)->inputFrame = new InputFrame();
			((MedianProcesser*)handle)->inputFrame->Datas = new short[frame->SampleCount];
		}
		auto ths_data = new float[frame->SampleCount];
		((CDFT*)((MedianProcesser*)handle)->fftOnRealTimeHandle)->DFT((sc16*)frame->Datas, nullptr, nullptr, ths_data, 0);
		ippsConvert_32f16s_Sfs(ths_data, ((MedianProcesser*)handle)->inputFrame->Datas, frame->SampleCount, ippRndNear, 0);
		delete[] ths_data;
		((MedianProcesser*)handle)->inputFrame->DetectLimitSNR = frame->DetectLimitSNR;
		((MedianProcesser*)handle)->inputFrame->HandleDataLength = frame->HandleDataLength;
		((MedianProcesser*)handle)->inputFrame->Resolution = (float)(frame->SampleRate) / (float)(frame->SampleCount);
		((MedianProcesser*)handle)->inputFrame->StartFrequency = frame->CenterFrequency - frame->ValidBandWidth / 2;
		((MedianProcesser*)handle)->inputFrame->FootLength = 1;
		int overLength = (frame->SampleRate - frame->ValidBandWidth) / ((MedianProcesser*)handle)->inputFrame->Resolution;
		((MedianProcesser*)handle)->inputFrame->SampleCount = frame->SampleCount - overLength;
		ippsCopy_16s(((MedianProcesser*)handle)->inputFrame->Datas + overLength / 2, ((MedianProcesser*)handle)->inputFrame->Datas, ((MedianProcesser*)handle)->inputFrame->SampleCount);
		return GetThresholdline(handle, ((MedianProcesser*)handle)->inputFrame);
	}

	void Dispose(void* handle)
	{
		if (handle == nullptr || !(MedianProcesser*)handle)return;
		delete (MedianProcesser*)handle;
	}
}