#pragma once
#include <Windows.h>
#include <atltypes.h>
#include <chrono>
using namespace std::chrono;

// ��ȡ��ǰʱ��
steady_clock::time_point GetNow();

// ˢ��ʱ��
void FlushSteadyClock();

// ����ʱ����뵱ǰʱ��ļ�������룩
LONGLONG GetInterval(steady_clock::time_point nLast);

// ���ô��ھ���
void WindowCenter(HWND hWnd);