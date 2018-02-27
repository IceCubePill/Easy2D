#include "..\emanagers.h"
#include "..\ebase.h"
#include "..\etransitions.h"
#include <stack>

static e2d::Scene * s_pCurrentScene = nullptr;
static e2d::Scene * s_pNextScene = nullptr;
static e2d::Transition * s_pTransition = nullptr;
static std::stack<e2d::Scene*> s_SceneStack;

void e2d::SceneManager::enterScene(Scene * scene, Transition * transition /* = nullptr */, bool saveCurrentScene /* = true */)
{
	ASSERT(scene != nullptr, "Next scene NULL pointer exception!");
	scene->retain();

	// ������һ������ָ��
	s_pNextScene = scene;
	
	// �����л���������
	if (transition)
	{
		s_pTransition = transition;
		transition->retain();
		transition->_setTarget(
			s_pCurrentScene,
			s_pNextScene
		);
	}

	if (s_pCurrentScene)
	{
		s_pCurrentScene->m_bWillSave = saveCurrentScene;
	}
}

void e2d::SceneManager::backScene(Transition * transition /* = nullptr */)
{
	// ջΪ��ʱ�����÷��س�������ʧ��
	WARN_IF(s_SceneStack.size() == 0, "Scene stack now is empty!");
	if (s_SceneStack.size() == 0) return;

	// ��ջ��ȡ������ָ�룬��Ϊ��һ����
	s_pNextScene = s_SceneStack.top();
	s_SceneStack.pop();

	// ������һ����ʱ�������浱ǰ����
	if (s_pCurrentScene)
	{
		s_pCurrentScene->m_bWillSave = false;
	}

	// �����л���������
	if (transition)
	{
		s_pTransition = transition;
		transition->retain();
		transition->_setTarget(
			s_pCurrentScene,
			s_pNextScene
		);
	}
}

void e2d::SceneManager::clearScene()
{
	// ��ճ���ջ
	while (s_SceneStack.size())
	{
		auto temp = s_SceneStack.top();
		SafeRelease(&temp);
		s_SceneStack.pop();
	}
}

e2d::Scene * e2d::SceneManager::getCurrentScene()
{
	return s_pCurrentScene;
}

bool e2d::SceneManager::isTransitioning()
{
	return s_pTransition != nullptr;
}

void e2d::SceneManager::__update()
{
	// ���³�������
	if (s_pCurrentScene)
	{
		s_pCurrentScene->_update();
	}

	// �����л�����ʱ��ִ�г����л�����
	if (s_pTransition)
	{
		// ���³�������
		if (s_pNextScene)
		{
			s_pNextScene->_update();
		}
		// ���³�������
		s_pTransition->_update();

		if (s_pTransition->isEnding())
		{
			s_pTransition->release();
			s_pTransition = nullptr;
		}
		else
		{
			return;
		}
	}

	// ��һ����ָ�벻Ϊ��ʱ���л�����
	if (s_pNextScene)
	{
		// ִ�е�ǰ������ onExit ����
		s_pCurrentScene->onExit();

		// ��Ҫ���浱ǰ��������������ջ��
		if (s_pCurrentScene->m_bWillSave)
		{
			s_SceneStack.push(s_pCurrentScene);
		}
		else
		{
			SafeRelease(&s_pCurrentScene);
		}

		// ִ����һ������ onEnter ����
		s_pNextScene->onEnter();

		s_pCurrentScene = s_pNextScene;		// �л�����
		s_pNextScene = nullptr;				// ��һ�����ÿ�
	}
}

void e2d::SceneManager::__render()
{
	// ���Ƶ�ǰ����
	if (s_pCurrentScene)
	{
		s_pCurrentScene->_render();
	}
	// �л�����ʱ��ͬʱ����������
	if (s_pTransition && s_pNextScene)
	{
		s_pNextScene->_render();
	}
}

bool e2d::SceneManager::__init()
{
	if (!s_pNextScene)
	{
		s_pNextScene = new Scene();
	}

	s_pCurrentScene = s_pNextScene;
	s_pCurrentScene->onEnter();
	s_pNextScene = nullptr;
	return true;
}

void e2d::SceneManager::__uninit()
{
	SafeRelease(&s_pCurrentScene);
	SafeRelease(&s_pNextScene);
	SafeRelease(&s_pTransition);
	SceneManager::clearScene();
}