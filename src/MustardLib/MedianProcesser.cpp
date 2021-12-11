#include "MedianProcesser.h"


void MedianProcesser::PeakOutSignal()
{
	_counting = false;
	float startPosition = 0;
	float stopPosition = 0;
	int maxIndex = 0;
	float maxValue = -200;
	for (int index = _entity->MedianFilteringFrameLength / 2 + 1; index < _entity->SampleCount - _entity->MedianFilteringFrameLength / 2 - 1; index++) {
		// 与门限值比较
		short minusRes = _entity->OrigionData[index] - _entity->MedianFilteringData[index];
		if (minusRes > 0) {
			if (minusRes > _snr) {
				if (_counting) {
					if (_entity->OrigionData[index] > maxValue) {
						maxIndex = index;
						maxValue = _entity->OrigionData[index];
					}
				}
				else {
					// 第一次出现位置
					startPosition = index - 1;
					maxValue = _entity->OrigionData[index];
					// 记录连续大于门限的最大位置
					maxIndex = index;
					_counting = true;
				}
			}
		}
		else {
			if (_counting && _entity->OrigionData[++index] <= _entity->MedianFilteringData[index] && startPosition > 0) {
				// 最后一次出现位置
				stopPosition = index - 1;
				float tre = (_entity->OrigionData[maxIndex] + 2 * _entity->MedianFilteringData[maxIndex]) / 3;
				// x = x1 + ((y - y1) * (x1 - x2)) / (y1 - y2)

				// 正向寻找截断交点 P1
				for (int i = startPosition; i < stopPosition; i++) {
					if (_entity->OrigionData[i] >= tre) {
						if (_entity->OrigionData[i] == tre) startPosition = i;
						else {
							if (_entity->OrigionData[i] == _entity->OrigionData[i - 1]) startPosition = i + 0.5;
							else startPosition = (float)i + (tre - (float)_entity->OrigionData[i]) / ((float)_entity->OrigionData[i] - (float)_entity->OrigionData[i - 1]);
						}
						// 反向寻找截断交点 P2
						for (int j = stopPosition; j > startPosition; j--) {
							if (_entity->OrigionData[j] == tre) stopPosition = j;
							else if (_entity->OrigionData[j] > tre) {
								if (_entity->OrigionData[j] == _entity->OrigionData[j + 1]) stopPosition = j - 0.5;
								else stopPosition = j + 1.0f + (tre - (float)_entity->OrigionData[j + 1]) / ((float)_entity->OrigionData[j + 1] - (float)_entity->OrigionData[j]);
							}
							else continue;
							break;
						}
						break;
					}
				}
				if (_signalCount < _maxSignalCount) {
					_outputsVecs[_signalCount].Bandwidth = (INT64)_entity->resolution * ((INT64)stopPosition - (INT64)startPosition);
					_outputsVecs[_signalCount].CenterFreq = _entity->startFrequency + (INT64)(_entity->resolution * (startPosition + stopPosition) / 2);
					_outputsVecs[_signalCount].AMP = maxValue;
					_outputsVecs[_signalCount].SNR = _entity->OrigionData[maxIndex] - _entity->MedianFilteringData[maxIndex];
					_signalCount++;
				}
				_counting = false;
			}
		}
	}
}

/// <summary>
/// 计算门限
/// </summary>
/// <returns>门限值</returns>
bool MedianProcesser::Compute(bool isOutputDetectResult, short* rData)
{
	float maxValue;
	float minValue;
	double sum = 0;
	size_t sumTime = 0;
	float lastAddValue = 0;
	float shiftValue = 0;

	// 简述：
	//		申请空间执行中值滤波。
	// 详情：
	//		取头尾去掉滑窗一半数据长度的原始数据进行中值滤波计算。
	if (_handleBuffer == nullptr) {
		int pBufferSize;
		ippsFilterMedianGetBufferSize(_entity->MedianFilteringFrameLength, ipp16s, &pBufferSize);
		_handleBuffer = ippsMalloc_8u(pBufferSize);
	}
	ippsFilterMedian_16s(_entity->OrigionData, _entity->MedianFilteringData, _entity->SampleCount, _entity->MedianFilteringFrameLength, 0, 0, _handleBuffer);

	// 简述：
	//		自适应门限计算。
	// 详情：
	//		取1和原始数据与中值的差值之间的最大值作为初始点，后续做原始值与中值的差值，若
	//		差值大于前序值的平均值（sum / sumTime），则计算其与前序平均值的比值的两倍作为
	//		放大权值，若差值小于前序值的平均值并大于0，若计算其与前序平均值的比值的两倍作
	//		为收束权值。
	for (INT64 i = _entity->MedianFilteringFrameLength / 2 + (INT64)1; i < (INT64)_entity->SampleCount - _entity->MedianFilteringFrameLength / 2 - (INT64)1; i++) {
		if (sumTime == 0) lastAddValue = max(_entity->OrigionData[i] - _entity->MedianFilteringData[i], 1);
		else {
			if (_entity->OrigionData[i] - (INT64)_entity->MedianFilteringData[i] > (sum / sumTime)) {
				lastAddValue = ((_entity->OrigionData[i] - (INT64)_entity->MedianFilteringData[i]) / (sum / sumTime)) * sum * 2.0 / sumTime;
			}
			else {
				if (_entity->OrigionData[i] - _entity->MedianFilteringData[i] > 0) {
					lastAddValue -= (((INT64)_entity->OrigionData[i] - _entity->MedianFilteringData[i]) / (sum / sumTime)) * sum / 2.0 / sumTime;
				}
				else {
					lastAddValue = sum / sumTime;
				}
			}
		}
		sum += lastAddValue;
		sumTime++;
	}
	// 简述:
	//		计算合值平均值。
	// 详情：
	//		见上描述。
	shiftValue = 2 * sum / sumTime;

	// 简述：
	//		计算自适应门限值。
	// 详情：
	//		此处采用合值平均值抬高两倍加上中值滤波后的值作为底噪门限。
	ippsAddC_16s_I(shiftValue, _entity->MedianFilteringData + _entity->MedianFilteringFrameLength / 2 + 1, _entity->SampleCount - _entity->MedianFilteringFrameLength / 2 - 1);

	ippsMove_16s(_entity->MedianFilteringData + _entity->MedianFilteringFrameLength / 2, _entity->MedianFilteringData, _entity->SampleCount - _entity->MedianFilteringFrameLength / 2);

	// 简述：
	//		检出信号。
	// 详情：
	//		与上一版本相同。
	if (isOutputDetectResult) {
		PeakOutSignal();
	}
	else {
		if (rData != nullptr) {
			memcpy(rData, _entity->MedianFilteringData, (size_t)_entity->SampleCount * 2);
		}
	}
	_onProcess = false;
	return true;
}

MedianProcesser::MedianProcesser(void* userData)
{
	_userData = userData;
}

/// <summary>
/// 释放句柄
/// </summary>
MedianProcesser::~MedianProcesser()
{
	_allowProcess = false;
	int i = 0;
	while (_onProcess && i < 10) {
		this_thread::sleep_for(chrono::milliseconds(10));
		i++;
	}
	if (_cutDownData != nullptr) {
		delete[] _cutDownData;
		_cutDownData = nullptr;
	}
	if (_cutThrsh != nullptr) {
		delete[] _cutThrsh;
		_cutThrsh = nullptr;
	}
	if (_tempArr != nullptr) {
		delete[] _tempArr;
		_tempArr = nullptr;
	}
	if (_entity != nullptr) {
		delete _entity;
		_entity = nullptr;
	}
	if (_handleBuffer != NULL) {
		ippsFree(_handleBuffer);
	}
	if (_outputsVecs != nullptr) {
		delete _outputsVecs;
	}
	Callback = nullptr;
}

/// <summary>
/// 入口函数
/// </summary>
/// <param name="inputFrame"></param>
/// <param name="isOutputDetectResult"></param>
/// <returns></returns>
bool MedianProcesser::InputData(InputFrame* inputFrame, short* rData, bool isOutputDetectResult)
{
	if (!_allowProcess) return false;
	_onProcess = true;
	if (_entity == nullptr) {
		_entity = new ThresholdHandleEntity;
		_entity->MedianFilteringData = new short[((INT64)inputFrame->SampleCount) + ((INT64)inputFrame->HandleDataLength - 1)];
	}
	if (_entity->resolution * _entity->SampleCount != inputFrame->SampleCount * inputFrame->Resolution) {
		if (_outputsVecs != nullptr) {
			delete _outputsVecs;
		}
		// 以 10k 一个信号计算信道最多信号数量
		_maxSignalCount = inputFrame->Resolution * inputFrame->SampleCount / 2'000;
		_outputsVecs = new OutputFrame[_maxSignalCount];
	}
	if (_entity->SampleCount != inputFrame->SampleCount) {
		_entity->SampleCount = inputFrame->SampleCount;
		if (_entity->OrigionData) delete[]_entity->OrigionData;
		_entity->OrigionData = new short[inputFrame->SampleCount];
		_peakPosition = 0;
		_movePosition = 0;
		if (_averageCache != nullptr)delete[] _averageCache;
		_averageCache = new double[inputFrame->SampleCount];
		ippsZero_64s((Ipp64s*)_averageCache, inputFrame->SampleCount);
#pragma omg parallel for
		for (size_t i = 0; i < 10; i++) {
			_dataCache[i] = new short[inputFrame->SampleCount];
			ippsZero_16s((Ipp16s*)_dataCache[i], inputFrame->SampleCount);
		}
	}
	_peakPosition = (_peakPosition + 1) > 9 ? 9 : (_peakPosition + 1);
	for (size_t i = 0; i < inputFrame->SampleCount; i++) {
		_averageCache[i] -= _dataCache[_movePosition][i] / 10.0;
		_dataCache[_movePosition][i] = inputFrame->Datas[i];
		_averageCache[i] += inputFrame->Datas[i] / 10.0;
		_entity->OrigionData[i] = (short)_averageCache[i];
	}
	_movePosition = (_movePosition + 1) % 10;
	if (_peakPosition < 9) {
		return true;
	}
	_entity->MedianFilteringFrameLength = inputFrame->HandleDataLength;
	_entity->resolution = inputFrame->Resolution;
	_entity->startFrequency = inputFrame->StartFrequency;
	_snr = inputFrame->DetectLimitSNR;
	_footLength = inputFrame->FootLength;
	auto res = Compute(isOutputDetectResult, rData);
	if (isOutputDetectResult && _signalCount > 0) {
		Callback(_outputsVecs, _signalCount, _userData);
		_signalCount = 0;
	}
	return res;
}
