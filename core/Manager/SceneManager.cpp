#include "..\emanagers.h"
#include "..\ebase.h"
#include "..\etransitions.h"

static e2d::Scene * s_pCurrentScene = nullptr;
static e2d::Scene * s_pNextScene = nullptr;
static e2d::Transition * s_pTransition = nullptr;
static std::stack<e2d::Scene*> s_SceneStack;

void e2d::SceneManager::enter(Scene * scene, Transition * transition /* = nullptr */, bool saveCurrentScene /* = true */)
{
	ASSERT(scene != nullptr, "Next scene NULL pointer exception!");
	scene->retain();

	// 保存下一场景的指针
	s_pNextScene = scene;
	
	// 设置切换场景动画
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

void e2d::SceneManager::back(Transition * transition /* = nullptr */)
{
	// 栈为空时，调用返回场景函数失败
	WARN_IF(s_SceneStack.size() == 0, "Scene stack now is empty!");
	if (s_SceneStack.size() == 0) return;

	// 从栈顶取出场景指针，作为下一场景
	s_pNextScene = s_SceneStack.top();
	s_SceneStack.pop();

	// 返回上一场景时，不保存当前场景
	if (s_pCurrentScene)
	{
		s_pCurrentScene->m_bWillSave = false;
	}

	// 设置切换场景动画
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

void e2d::SceneManager::clear()
{
	// 清空场景栈
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

std::stack<e2d::Scene*> e2d::SceneManager::getSceneStack()
{
	return s_SceneStack;
}

bool e2d::SceneManager::isTransitioning()
{
	return s_pTransition != nullptr;
}

void e2d::SceneManager::__update()
{
	// 更新场景内容
	if (s_pCurrentScene)
	{
		s_pCurrentScene->_update();
	}

	// 正在切换场景时，执行场景切换动画
	if (s_pTransition)
	{
		// 更新场景内容
		if (s_pNextScene)
		{
			s_pNextScene->_update();
		}
		// 更新场景动画
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

	// 下一场景指针不为空时，切换场景
	if (s_pNextScene)
	{
		// 执行当前场景的 onExit 函数
		s_pCurrentScene->onExit();

		// 若要保存当前场景，把它放入栈中
		if (s_pCurrentScene->m_bWillSave)
		{
			s_SceneStack.push(s_pCurrentScene);
		}
		else
		{
			SafeRelease(&s_pCurrentScene);
		}

		// 执行下一场景的 onEnter 函数
		s_pNextScene->onEnter();

		s_pCurrentScene = s_pNextScene;		// 切换场景
		s_pNextScene = nullptr;				// 下一场景置空
	}
}

void e2d::SceneManager::__render()
{
	// 绘制当前场景
	if (s_pCurrentScene)
	{
		s_pCurrentScene->_render();
	}
	// 切换场景时，同时绘制两场景
	if (s_pTransition && s_pNextScene)
	{
		s_pNextScene->_render();
	}
}

bool e2d::SceneManager::__init()
{
	// 若游戏初始化时场景不为空，进入该场景
	if (s_pNextScene)
	{
		s_pCurrentScene = s_pNextScene;
		s_pCurrentScene->onEnter();
		s_pNextScene = nullptr;
	}

	// 更新场景内容
	SceneManager::__update();

	return true;
}

void e2d::SceneManager::__uninit()
{
	SafeRelease(&s_pCurrentScene);
	SafeRelease(&s_pNextScene);
	SafeRelease(&s_pTransition);
	SceneManager::clear();
}
