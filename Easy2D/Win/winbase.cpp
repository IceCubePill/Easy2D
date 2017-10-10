#include "winbase.h"

static steady_clock::time_point nNow;

steady_clock::time_point GetNow()
{
	return nNow;
}

void FlushSteadyClock()
{
	nNow = steady_clock::now();	// ��ȡ��ǰʱ��
}

LONGLONG GetInterval(steady_clock::time_point nLast)
{
	return duration_cast<milliseconds>(nNow - nLast).count();
}

void WindowCenter(HWND hWnd)
{
	// ��ȡ��Ļ�ֱ���
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// ��ȡ���ڴ�С
	CRect rcWindow;
	GetWindowRect(hWnd, &rcWindow);
	// ���ô�������Ļ����
	SetWindowPos(hWnd, HWND_TOP,
		(screenWidth - rcWindow.Size().cx) / 2,
		(screenHeight - rcWindow.Size().cy) / 2,
		rcWindow.Size().cx,
		rcWindow.Size().cy,
		SWP_SHOWWINDOW | SWP_NOSIZE);
}