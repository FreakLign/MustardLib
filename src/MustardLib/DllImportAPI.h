#pragma once
typedef short* shorts;
#define MDEF(type, name ,value) typedef type name value
typedef void* MDHANDLE;

// ����ṹ��
typedef struct OutputFrame {
	long long	CenterFreq;		// ����Ƶ��
	int			Bandwidth;		// ����
	float		SNR;			// �����
	float		AMP;			// ����
}*POutputFrame;

// ����֡�ṹ��Ƶ�����ݣ�
typedef struct InputFrame {
	int				DataLength;				// ���ݳ���
	int				HandleDataLength;		// ���޴���������
	long long		StartFrequency;			// ��ʼƵ��(Hz)
	int				FootLength;				// �ɵ㲽��
	float			Resolution;				// ����ֱ���
	float			DetectLimitSNR;			// ��������
	shorts			Datas;					// ����֡����
}*PInputFrame;

// ����֡�ṹ��IQ���ݣ�
typedef struct IQInputFrame {
	int				DataLength;				// ������
	int				HandleDataLength;		// ���޴���������
	long long 		CenterFrequency;		// ����Ƶ��
	long long		SampleRate;				// ������
	long long		ValidBandWidth;			// ��Ч����
	long long		DataTimeStamp;			// ʱ���
	float			DetectLimitSNR;			// ��������
	shorts			Datas;					// ����֡����
}*PIQInputFrame;

// ������ص�
// Para 0	���ص����ָ��
// Para 1	���ص��������
typedef void __stdcall DetectResultAllBack(POutputFrame, int), *PDetectResultAllBack;

// ������⴦��ʵ��
// callback	��������ص�����ָ��
// ����ֵ	������ʵ�����
MDEF(MDHANDLE, CreateHandle, (PDetectResultAllBack callback));

// ��Ƶ�����ݼ���ź�
// handle	������ʵ��
// frame	����������֡
// ����ֵ	��ִ�����
MDEF(bool, Compute, (MDHANDLE handle, PInputFrame frame));

// ��IQ���ݼ���ź�
// handle	������ʵ��
// frame	����������֡
// ����ֵ	��ִ�����
MDEF(bool, ExComputeIQ, (MDHANDLE handle, PIQInputFrame frame));

// �ͷ�ʵ��
// handle	������ʵ��
MDEF(void, Dispose, (MDHANDLE handle));