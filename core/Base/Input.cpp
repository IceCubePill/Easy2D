#include "..\ebase.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace e2d;

static IDirectInput8* s_pDirectInput = nullptr;			// DirectInput �ӿڶ���
static IDirectInputDevice8* s_KeyboardDevice = nullptr;	// �����豸�ӿ�
static char s_KeyBuffer[256] = { 0 };					// ���ڱ�����̰�����Ϣ������
static char s_KeyRecordBuffer[256] = { 0 };				// ������Ϣ����������

static IDirectInputDevice8* s_MouseDevice = nullptr;	// ����豸�ӿ�
static DIMOUSESTATE s_MouseState;						// �����Ϣ�洢�ṹ��
static DIMOUSESTATE s_MouseRecordState;					// �����Ϣ��������
static POINT s_MousePosition;							// ���λ�ô洢�ṹ��


void Input::__uninit()
{
	if (s_KeyboardDevice)
		s_KeyboardDevice->Unacquire();
	if (s_MouseDevice)
		s_MouseDevice->Unacquire();

	SafeReleaseInterface(&s_MouseDevice);
	SafeReleaseInterface(&s_KeyboardDevice);
	SafeReleaseInterface(&s_pDirectInput);
}

bool Input::__init()
{
	ZeroMemory(s_KeyBuffer, sizeof(s_KeyBuffer));
	ZeroMemory(s_KeyRecordBuffer, sizeof(s_KeyRecordBuffer));
	ZeroMemory(&s_MouseState, sizeof(s_MouseState));
	ZeroMemory(&s_MouseRecordState, sizeof(s_MouseRecordState));

	// ��ʼ���ӿڶ���
	HRESULT hr = DirectInput8Create(
		HINST_THISCOMPONENT,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&s_pDirectInput,
		nullptr
	);

	if (SUCCEEDED(hr))
	{
		// ��ʼ�������豸
		hr = s_pDirectInput->CreateDevice(
			GUID_SysKeyboard,
			&s_KeyboardDevice,
			nullptr
		);

		if (SUCCEEDED(hr))
		{
			s_KeyboardDevice->SetCooperativeLevel(
				Window::getHWnd(),
				DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
			);
			s_KeyboardDevice->SetDataFormat(
				&c_dfDIKeyboard);
			s_KeyboardDevice->Acquire();
			s_KeyboardDevice->Poll();
		}
		else
		{
			MessageBox(nullptr, L"Keyboard not found. The game will now exit.",
				L"Error",
				MB_ICONERROR | MB_OK);
			Game::quit();
			return false;
		}
	}

	if (SUCCEEDED(hr))
	{
		// ��ʼ������豸
		hr = s_pDirectInput->CreateDevice(GUID_SysMouse, &s_MouseDevice, nullptr);

		if (SUCCEEDED(hr))
		{
			s_MouseDevice->SetCooperativeLevel(Window::getHWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			s_MouseDevice->SetDataFormat(&c_dfDIMouse);
			s_MouseDevice->Acquire();
			s_MouseDevice->Poll();
		}
		else
		{
			MessageBox(nullptr, L"Mouse not found. The game will now exit.",
				L"Error",
				MB_ICONERROR | MB_OK);
			Game::quit();
			return false;
		}
	}

	return SUCCEEDED(hr);
}

void Input::__updateDeviceState()
{
	if (s_KeyboardDevice)
	{
		HRESULT hr = s_KeyboardDevice->Poll();
		if (FAILED(hr))
		{
			hr = s_KeyboardDevice->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = s_KeyboardDevice->Acquire();
		}
		else
		{
			for (int i = 0; i < 256; i++)
				s_KeyRecordBuffer[i] = s_KeyBuffer[i];

			s_KeyboardDevice->GetDeviceState(sizeof(s_KeyBuffer), (void**)&s_KeyBuffer);
		}
	}

	if (s_MouseDevice)
	{
		HRESULT hr = s_MouseDevice->Poll();
		if (FAILED(hr))
		{
			hr = s_MouseDevice->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = s_MouseDevice->Acquire();
		}
		else
		{
			s_MouseRecordState = s_MouseState;
			s_MouseDevice->GetDeviceState(sizeof(s_MouseState), (void**)&s_MouseState);
		}
		DIK_0;
	}

	GetCursorPos(&s_MousePosition);
	ScreenToClient(Window::getHWnd(), &s_MousePosition);
}

bool Input::isKeyDown(int nKeyCode)
{
	if (s_KeyBuffer[nKeyCode] & 0x80)
		return true;
	return false;
}

bool Input::isKeyPress(int nKeyCode)
{
	if ((s_KeyBuffer[nKeyCode] & 0x80) && !(s_KeyRecordBuffer[nKeyCode] & 0x80))
		return true;
	return false;
}

bool Input::isKeyRelease(int nKeyCode)
{
	if (!(s_KeyBuffer[nKeyCode] & 0x80) && (s_KeyRecordBuffer[nKeyCode] & 0x80))
		return true;
	return false;
}

bool Input::isMouseLButtonDown()
{
	if (s_MouseState.rgbButtons[0] & 0x80)
		return true;
	return false;
}

bool Input::isMouseRButtonDown()
{
	if (s_MouseState.rgbButtons[1] & 0x80)
		return true;
	return false;
}

bool Input::isMouseMButtonDown()
{
	if (s_MouseState.rgbButtons[2] & 0x80)
		return true;
	return false;
}

bool Input::isMouseLButtonPress()
{
	if ((s_MouseState.rgbButtons[0] & 0x80) && !(s_MouseRecordState.rgbButtons[0] & 0x80))
		return true;
	return false;
}

bool Input::isMouseRButtonPress()
{
	if ((s_MouseState.rgbButtons[1] & 0x80) && !(s_MouseRecordState.rgbButtons[1] & 0x80))
		return true;
	return false;
}

bool Input::isMouseMButtonPress()
{
	if ((s_MouseState.rgbButtons[2] & 0x80) && !(s_MouseRecordState.rgbButtons[2] & 0x80))
		return true;
	return false;
}

bool Input::isMouseLButtonRelease()
{
	if (!(s_MouseState.rgbButtons[0] & 0x80) && (s_MouseRecordState.rgbButtons[0] & 0x80))
		return true;
	return false;
}

bool Input::isMouseRButtonRelease()
{
	if (!(s_MouseState.rgbButtons[1] & 0x80) && (s_MouseRecordState.rgbButtons[1] & 0x80))
		return true;
	return false;
}

bool Input::isMouseMButtonRelease()
{
	if (!(s_MouseState.rgbButtons[2] & 0x80) && (s_MouseRecordState.rgbButtons[2] & 0x80))
		return true;
	return false;
}

double Input::getMouseX()
{
	return (double)s_MousePosition.x;
}

double Input::getMouseY()
{
	return (double)s_MousePosition.y;
}

Point Input::getMousePos()
{
	return Point((double)s_MousePosition.x, (double)s_MousePosition.y);
}

double Input::getMouseDeltaX()
{
	return (double)s_MouseState.lX;
}

double Input::getMouseDeltaY()
{
	return (double)s_MouseState.lY;
}

double Input::getMouseDeltaZ()
{
	return (double)s_MouseState.lZ;
}