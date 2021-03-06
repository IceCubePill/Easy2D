#include "..\emanagers.h"
#include "..\ebase.h"

// ObjectManager 释放池的实现机制：
// Object 类中的引用计数（m_nRefCount）保证了指针的使用安全
// 它记录了对象被使用的次数，当计数为 0 时，ObjectManager 会自动释放这个对象
// 所有的 Object 对象都应在被使用时（例如 Text 添加到了场景中）
// 调用 retain 函数保证该对象不被删除，并在不再使用时调用 release 函数
// 让其自动释放

// 对象管理池
static std::vector<e2d::Object*> s_vObjectPool;
// 标志释放池执行状态
static bool s_bNotifyed = false;

void e2d::ObjectManager::__update()
{
	if (!s_bNotifyed) return;

	s_bNotifyed = false;
	// 创建迭代器
	static std::vector<e2d::Object*>::iterator iter;
	// 循环遍历容器中的所有对象
	for (iter = s_vObjectPool.begin(); iter != s_vObjectPool.end();)
	{
		if ((*iter)->getReferenceCount() <= 0)
		{
			// 若对象的引用的计数小于等于 0, 释放该对象
			delete (*iter);
			// 从释放池中删除该对象
			iter = s_vObjectPool.erase(iter);
		}
		else
		{
			iter++;		// 移动迭代器
		}
	}
}

void e2d::ObjectManager::__uninit()
{
	// 释放两遍内存
	s_bNotifyed = true;
	ObjectManager::__update();

	s_bNotifyed = true;
	ObjectManager::__update();
}

void e2d::ObjectManager::add(e2d::Object * nptr)
{
	if (!nptr->m_bManaged)
	{
		nptr->m_bManaged = true;
		s_vObjectPool.push_back(nptr);	// 将一个对象放入释放池中
	}
}

void e2d::ObjectManager::clear()
{
	s_bNotifyed = true;
}
