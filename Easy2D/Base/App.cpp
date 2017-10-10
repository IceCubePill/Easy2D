#include "..\easy2d.h"
#include "..\Win\winbase.h"
#include "..\EasyX\easyx.h"
#include <assert.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#include <stack>
#include <chrono>
#include <thread>

using namespace std::chrono;

// App ��Ψһʵ��
static App * s_pInstance = nullptr;
// ����ջ
static std::stack<Scene*> s_SceneStack;

App::App() : 
	m_pCurrentScene(nullptr), 
	m_pNextScene(nullptr), 
	m_pLoadingScene(nullptr),
	m_bRunning(false), 
	m_nWindowMode(0),
	m_bSaveScene(true)
{
	assert(!s_pInstance);	// ����ͬʱ�������� App ʵ��
	s_pInstance = this;		// ����ʵ������
}

App::~App()
{
}

App * App::get()
{
	assert(s_pInstance);	// ����ʵ������
	return s_pInstance;		// ��ȡ App ��Ψһʵ��
}

int App::run()
{
	// ��¼��ǰʱ��
	steady_clock::time_point nLast = steady_clock::now();
	// ֡���
	LONGLONG nAnimationInterval = 17LL;
	// ʱ����
	LONGLONG nInterval = 0LL;
	// ����ʱ��
	LONGLONG nWaitMS = 0LL;

	// ��ʼ������
	_initGraph();
	// ����������ͼ
	BeginBatchDraw();
	// ����һ�λ���
	_draw();
	// ������Ϸ
	m_bRunning = true;

	// ������ѭ��
	while (m_bRunning)
	{
		// ˢ�¼�ʱ
		::FlushSteadyClock();
		// ����ʱ����
		nInterval = GetInterval(nLast);
		// �жϼ��ʱ���Ƿ��㹻
		if (nInterval >= nAnimationInterval)
		{
			// ��¼��ǰʱ��
			nLast += milliseconds(nAnimationInterval);
			// ˢ����Ϸ����
			_draw();
			// ִ����ѭ��
			_mainLoop();
		}
		else
		{
			// �������ʱ��
			nWaitMS = nAnimationInterval - nInterval - 1;
			// �����̣߳��ͷ� CPU ռ��
			if (nWaitMS > 1LL)
			{
				std::this_thread::sleep_for(milliseconds(nWaitMS));
			}
		}
	}
	// ֹͣ������ͼ
	EndBatchDraw();
	// �رմ���
	close();
	// �ͷ������ڴ�ռ��
	free();

	return 0;
}

void App::_initGraph()
{
	// ������ͼ����
	initgraph(m_Size.cx, m_Size.cy, m_nWindowMode);
	// ���ô��ھ���
	WindowCenter(GetHWnd());
	// �������뷨
	ImmAssociateContext(GetHWnd(), NULL);
	// ���û�ͼ����
	reset();
	// ���ô��ڱ���
	if (m_sTitle.empty())
	{
		// ���浱ǰ����
		TCHAR title[31];
		GetWindowText(GetHWnd(), title, 30);
		m_sTitle = title;
	}
	else
	{
		setWindowTitle(m_sTitle);
	}
	// ��δ���� AppName��Ĭ����Ϊ������
	if (m_sAppName.empty())
	{
		m_sAppName = m_sTitle;
	}
}

void App::_draw()
{
	// ��һ����ָ�벻Ϊ��ʱ���л�����
	if (m_pNextScene)
	{
		// ������һ����
		_enterNextScene();
	}
	cleardevice();				// ��ջ���
	m_pCurrentScene->_onDraw();	// ���Ƶ�ǰ����
	FlushBatchDraw();			// ˢ�»���
}

void App::_mainLoop()
{
	MouseMsg::__exec();			// �����
	KeyMsg::__exec();			// ���̰������
	Timer::__exec();			// ��ʱ��ִ�г���
	ActionManager::__exec();	// ����������ִ�г���
	FreePool::__flush();		// ˢ���ڴ��
}

void App::createWindow(int width, int height, int mode)
{
	// ���洰����Ϣ
	m_Size.cx = width;
	m_Size.cy = height;
	m_nWindowMode = mode;
}

void App::createWindow(CSize size, int mode)
{
	createWindow(size.cx, size.cy, mode);
}

void App::createWindow(TString title, int width, int height, int mode)
{
	// ���洰����Ϣ
	m_Size.cx = width;
	m_Size.cy = height;
	m_nWindowMode = mode;
	m_sTitle = title;
}

void App::createWindow(TString title, CSize size, int mode)
{
	createWindow(title, size.cx, size.cy, mode);
}

void App::setWindowSize(int width, int height)
{
	// �޸Ĵ��ڴ�С
	Resize(NULL, width, height);
	// ���ô��ھ���
	WindowCenter(GetHWnd());
	// ���ô�������
	reset();
}

void App::setWindowSize(CSize size)
{
	setWindowSize(size.cx, size.cy);
}

void App::setWindowTitle(TString title)
{
	// ���ô��ڱ���
	SetWindowText(GetHWnd(), title.c_str());
	// ���浱ǰ���⣬�����޸Ĵ��ڴ�Сʱ�ָ�����
	s_pInstance->m_sTitle = title;
}

TString App::getWindowTitle()
{
	return s_pInstance->m_sTitle;
}

void App::close()
{
	closegraph();	// �رջ�ͼ����
}

void App::enterScene(Scene * scene, bool save)
{
	// ������һ������ָ��
	s_pInstance->m_pNextScene = scene;
	// �л�����ʱ���Ƿ񱣴浱ǰ����
	s_pInstance->m_bSaveScene = save;
}

void App::backScene()
{
	// ��ջ��ȡ������ָ�룬��Ϊ��һ����
	s_pInstance->m_pNextScene = s_SceneStack.top();
	// �����浱ǰ����
	s_pInstance->m_bSaveScene = false;
}

void App::clearScene()
{
	// ��ճ���ջ
	while (s_SceneStack.size())
	{
		auto temp = s_SceneStack.top();
		SafeDelete(temp);
		s_SceneStack.pop();
	}
}

void App::setAppName(TString appname)
{
	s_pInstance->m_sAppName = appname;
}

TString App::getAppName()
{
	return s_pInstance->m_sAppName;
}

void App::setBkColor(COLORREF color)
{
	setbkcolor(color);
}

void App::_enterNextScene()
{
	bool bBackScene = false;

	// ����һ��������ջ����˵�����ڷ�����һ����
	if (s_SceneStack.size() && m_pNextScene == s_SceneStack.top())
	{
		bBackScene = true;
		// ɾ��ջ������
		s_SceneStack.pop();
	}

	// ִ�е�ǰ������ onExit ����
	if (m_pCurrentScene)
	{
		m_pCurrentScene->onExit();
		if (m_bSaveScene)
		{
			// ��Ҫ���浱ǰ��������������ջ��
			s_SceneStack.push(m_pCurrentScene);
			// ��ͣ��ǰ���������е����ж�ʱ��
			Timer::waitAllSceneTimers(m_pCurrentScene);
			MouseMsg::waitAllSceneListeners(m_pCurrentScene);
			KeyMsg::waitAllSceneListeners(m_pCurrentScene);
			ActionManager::waitAllSceneActions(m_pCurrentScene);
		}
		else
		{
			// �����泡��ʱ��ֹͣ��ǰ���������е����ж�ʱ������ɾ����ǰ����
			Timer::clearAllSceneTimers(m_pCurrentScene);
			MouseMsg::clearAllSceneListeners(m_pCurrentScene);
			KeyMsg::clearAllSceneListeners(m_pCurrentScene);
			ActionManager::stopAllSceneActions(m_pCurrentScene);
			SafeDelete(m_pCurrentScene);
		}
	}

	m_pCurrentScene = m_pNextScene;		// �л�����
	m_pNextScene = nullptr;				// ��һ�����ÿ�

	if (bBackScene)
	{
		// ������һ����ʱ���ָ������ϵĶ�ʱ��
		Timer::notifyAllSceneTimers(m_pCurrentScene);
		MouseMsg::notifyAllSceneListeners(m_pCurrentScene);
		KeyMsg::notifyAllSceneListeners(m_pCurrentScene);
		ActionManager::notifyAllSceneActions(m_pCurrentScene);
	}
	else
	{
		m_pCurrentScene->init();		// ����һ���³���ʱ��ִ������ init ����
	}

	m_pCurrentScene->onEnter();			// ִ����һ������ onEnter ����
}

void App::quit()
{
	s_pInstance->m_bRunning = false;
}

void App::end()
{
	s_pInstance->m_bRunning = false;
}

void App::reset()
{
	// ���û�ͼ����
	graphdefaults();
	setbkmode(TRANSPARENT);
	setbkcolor(Color::black);
}

Scene * App::getCurrentScene()
{
	// ��ȡ��ǰ������ָ��
	return s_pInstance->m_pCurrentScene;
}

Scene * App::getLoadingScene()
{
	return s_pInstance->m_pLoadingScene;
}

int App::getWidth()
{
	return s_pInstance->m_Size.cx;
}

int App::getHeight()
{
	return s_pInstance->m_Size.cy;
}

void App::free()
{
	// �ͷų����ڴ�
	SafeDelete(m_pCurrentScene);
	SafeDelete(m_pNextScene);
	// ��ճ���ջ
	while (s_SceneStack.size())
	{
		auto temp = s_SceneStack.top();
		SafeDelete(temp);
		s_SceneStack.pop();
	}
	// ɾ�����ж�ʱ��
	Timer::clearAllTimers();
	MouseMsg::clearAllListeners();
	KeyMsg::clearAllListeners();
	ActionManager::clearAllActions();
	// ɾ�����ж���
	FreePool::__clearAllObjects();
}
