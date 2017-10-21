#pragma once
#include "ebase.h"
#include <chrono>
#include <random>

namespace e2d
{

class ETimerManager;
class EAction;

// 对象管理器
class EObjectManager
{
	friend EApp;

public:
	// 将一个节点放入内存池
	static void add(
		e2d::EObject * nptr
	);

	// 删除所有节点
	static void clearAllObjects();

	// 通知内存池刷新
	static void notifyFlush();

private:
	// 刷新内存池
	static void __flush();
};


// 定时器
class ETimer :
	public EObject
{
	friend ETimerManager;

public:
	ETimer();

	ETimer(
		const TIMER_CALLBACK &callback,	/* 定时器回调函数 */
		int repeatTimes = -1,			/* 定时器执行次数 */
		LONGLONG interval = 0LL,		/* 时间间隔（毫秒） */
		bool atOnce = false				/* 是否立即执行 */
	);

	ETimer(
		const EString &name,			/* 定时器名称 */
		const TIMER_CALLBACK &callback,	/* 定时器回调函数 */
		int repeatTimes = -1,			/* 定时器执行次数 */
		LONGLONG interval = 0LL,		/* 时间间隔（毫秒） */
		bool atOnce = false				/* 是否立即执行 */
	);

	// 获取定时器状态
	bool isRunning() const;

	// 启动定时器
	void start();

	// 停止定时器
	void stop();

	// 获取定时器名称
	EString getName() const;

	// 获取定时器所在场景
	EScene * getParentScene() const;

	// 获取定时器所在节点
	ENode * getParentNode() const;

	// 设置定时器名称
	void setName(
		const EString &name
	);

	// 设置定时器执行间隔
	void setInterval(
		LONGLONG interval
	);

	// 设置定时器回调函数
	void setCallback(
		const TIMER_CALLBACK & callback
	);

	// 设置定时器重复执行次数
	void setRepeatTimes(
		int repeatTimes
	);

	// 绑定定时器到场景
	virtual void bindWith(
		EScene * pParentScene
	);

	// 绑定定时器到节点
	virtual void bindWith(
		ENode * pParentNode
	);

protected:
	// 执行回调函数
	virtual void _callOn();

	// 判断是否达到执行状态
	bool _isReady();

protected:
	EString			m_sName;
	bool			m_bRunning;
	bool			m_bAtOnce;
	int				m_nRunTimes;
	int				m_nRepeatTimes;
	EScene *		m_pParentScene;
	ENode *			m_pParentNode;
	TIMER_CALLBACK	m_Callback;
	LONGLONG		m_nInterval;
	std::chrono::steady_clock::time_point m_tLast;
};


// 定时器管理器
class ETimerManager
{
	friend EApp;
	friend EScene;
	friend ENode;

public:
	// 绑定定时器到场景
	static void bindTimer(
		ETimer * timer,
		EScene * pParentScene
	);

	// 绑定定时器到节点
	static void bindTimer(
		ETimer * timer,
		ENode * pParentNode
	);

	// 启动具有相同名称的定时器
	static void startTimers(
		const EString &name
	);

	// 停止具有相同名称的定时器
	static void stopTimers(
		const EString &name
	);

	// 删除具有相同名称的定时器
	static void delTimers(
		const EString &name
	);

	// 启动绑定在场景及其子节点上的所有定时器
	static void startAllTimersBindedWith(
		EScene * pParentScene
	);

	// 停止绑定在场景及其子节点上的所有定时器
	static void stopAllTimersBindedWith(
		EScene * pParentScene
	);

	// 启动绑定在节点上的所有定时器
	static void startAllTimersBindedWith(
		ENode * pParentNode
	);

	// 停止绑定在节点上的所有定时器
	static void stopAllTimersBindedWith(
		ENode * pParentNode
	);

	// 启动所有定时器
	static void startAllTimers();

	// 停止所有定时器
	static void stopAllTimers();

private:
	// 清空定时器管理器
	static void _clearManager();

	// 清空绑定在场景及其子节点上的所有定时器
	static void _clearAllTimersBindedWith(
		EScene * pParentScene
	);

	// 清空绑定在节点上的所有定时器
	static void _clearAllTimersBindedWith(
		ENode * pParentNode
	);

	// 重置定时器状态
	static void _resetAllTimers();

	// 定时器执行程序
	static void TimerProc();
};


// 动作管理器
class EActionManager
{
	friend EApp;
	friend EScene;
	friend ENode;

public:
	// 添加动作
	static void addAction(
		EAction * action
	);

	// 继续绑定在节点上的所有动作
	static void startAllActionsBindedWith(
		ENode * pTargetNode
	);

	// 暂停绑定在节点上的所有动作
	static void pauseAllActionsBindedWith(
		ENode * pTargetNode
	);

	// 停止绑定在节点上的所有动作
	static void stopAllActionsBindedWith(
		ENode * pTargetNode
	);

	// 继续所有动作
	static void startAllActions();

	// 暂停所有动作
	static void pauseAllActions();

	// 停止所有动作
	static void stopAllActions();

private:
	// 清空动画管理器
	static void _clearManager();

	// 清空绑定在节点上的所有动作
	static void _clearAllActionsBindedWith(
		ENode * pTargetNode
	);

	// 重置所有动作状态
	static void _resetAllActions();

	// 动作执行程序
	static void ActionProc();
};


class EFileUtils
{
public:
	// 获取系统的 AppData\Local 路径
	static EString getLocalAppDataPath();

	// 获取默认的保存路径
	static EString getDefaultSavePath();

	// 保存 int 型的值
	static void saveInt(LPCTSTR key, int value);

	// 保存 double 型的值
	static void saveDouble(LPCTSTR key, double value);

	// 保存 字符串 型的值（不要在 Unicode 字符集下保存中文字符）
	static void saveString(LPCTSTR key, EString value);

	// 获取 int 型的值（若不存在则返回 default 参数的值）
	static int getInt(LPCTSTR key, int default);

	// 获取 double 型的值（若不存在则返回 default 参数的值）
	static double getDouble(LPCTSTR key, double default);

	// 获取 字符串 型的值（若不存在则返回 default 参数的值）
	static EString geTString(LPCTSTR key, EString default);

	// 得到文件扩展名（小写）
	static EString getFileExtension(const EString & filePath);

	/**
	*  打开保存文件对话框，得到有效保存路径返回 true
	*  参数：返回文件路径的字符串，窗口标题，设置扩展名过滤，设置默认扩展名
	*/
	static EString getSaveFilePath(LPCTSTR title = L"保存到", LPCTSTR defExt = NULL);
};


class ERandom
{
public:
	// 取得整型范围内的一个随机数
	template<typename T>
	static inline T between(T min, T max) { return e2d::ERandom::randomInt(min, max); }

	// 取得浮点数范围内的一个随机数
	static inline float between(float min, float max) { return e2d::ERandom::randomReal(min, max); }

	// 取得浮点数范围内的一个随机数
	static inline double between(double min, double max) { return e2d::ERandom::randomReal(min, max); }

	// 取得浮点数范围内的一个随机数
	static inline long double between(long double min, long double max) { return e2d::ERandom::randomReal(min, max); }

	// 取得整型范围内的一个随机数
	template<typename T>
	static T randomInt(
		T min, 
		T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(getEngine());
	}

	// 取得浮点数类型范围内的一个随机数
	template<typename T>
	static T randomReal(
		T min, 
		T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(getEngine());
	}

	// 获取随机数产生器
	static std::default_random_engine &getEngine();
};

}