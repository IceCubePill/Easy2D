#include "..\enodes.h"
#include "..\emanagers.h"
#include "..\etools.h"
#include "..\eactions.h"
#include "..\eshape.h"
#include <algorithm>

// Ĭ�����ĵ�λ��
static float s_fDefaultPiovtX = 0;
static float s_fDefaultPiovtY = 0;

e2d::Node::Node()
	: m_nOrder(0)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fWidth(0)
	, m_fHeight(0)
	, m_fScaleX(1.0f)
	, m_fScaleY(1.0f)
	, m_fRotation(0)
	, m_fSkewAngleX(0)
	, m_fSkewAngleY(0)
	, m_fDisplayOpacity(1.0f)
	, m_fRealOpacity(1.0f)
	, m_fPivotX(s_fDefaultPiovtX)
	, m_fPivotY(s_fDefaultPiovtY)
	, m_MatriInitial(D2D1::Matrix3x2F::Identity())
	, m_MatriFinal(D2D1::Matrix3x2F::Identity())
	, m_bVisiable(true)
	, m_bDisplayedInScene(false)
	, m_pShape(nullptr)
	, m_pParent(nullptr)
	, m_pParentScene(nullptr)
	, m_nHashName(0)
	, m_bSortChildrenNeeded(false)
	, m_bTransformNeeded(false)
	, m_bAutoUpdate(true)
{
}

e2d::Node::~Node()
{
	TimerManager::__clearAllTimersBindedWith(this);
	ActionManager::__clearAllActionsBindedWith(this);
	ShapeManager::__delShape(m_pShape);
	for (auto child : m_vChildren)
	{
		SafeRelease(&child);
	}
}

void e2d::Node::_update()
{
	if (m_bTransformNeeded)
	{
		_updateTransform(this);
	}

	if (!m_vChildren.empty())
	{
		if (m_bSortChildrenNeeded)
		{
			// �ӽڵ�����
			auto sortFunc = [](Node * n1, Node * n2) {
				return n1->getOrder() < n2->getOrder();
			};

			std::sort(
				std::begin(m_vChildren),
				std::end(m_vChildren),
				sortFunc
			);

			m_bSortChildrenNeeded = false;
		}

		// �����ӽڵ�
		size_t size = m_vChildren.size();
		size_t i;
		for (i = 0; i < size; i++)
		{
			auto child = m_vChildren[i];
			// ���� Order С����Ľڵ�
			if (child->getOrder() < 0)
			{
				child->_update();
			}
			else
			{
				break;
			}
		}

		if (m_bAutoUpdate)
		{
			if (!Game::isPaused())
			{
				this->onUpdate();
			}
			this->onFixedUpdate();
		}

		// ����ʣ��ڵ�
		for (; i < size; i++)
			m_vChildren[i]->_update();
	}
	else
	{
		if (m_bAutoUpdate)
		{
			if (!Game::isPaused())
			{
				this->onUpdate();
			}
			this->onFixedUpdate();
		}
	}
}

void e2d::Node::_render()
{
	if (!m_bVisiable)
	{
		return;
	}

	if (!m_vChildren.empty())
	{
		size_t size = m_vChildren.size();
		size_t i;
		for (i = 0; i < size; i++)
		{
			auto child = m_vChildren[i];
			// ���� Order С����Ľڵ�
			if (child->getOrder() < 0)
			{
				child->_render();
			}
			else
			{
				break;
			}
		}

		// ת����Ⱦ���Ķ�ά����
		Renderer::getRenderTarget()->SetTransform(m_MatriFinal);
		// ��Ⱦ����
		this->onRender();

		// ����ʣ��ڵ�
		for (; i < size; i++)
			m_vChildren[i]->_render();
	}
	else
	{
		// ת����Ⱦ���Ķ�ά����
		Renderer::getRenderTarget()->SetTransform(m_MatriFinal);
		// ��Ⱦ����
		this->onRender();
	}
}

void e2d::Node::_drawShape()
{
	// ���������ļ�����״
	if (m_pShape && m_pShape->m_bIsVisiable)
	{
		m_pShape->_render();
	}

	// ���������ӽڵ�ļ�����״
	for (auto child : m_vChildren)
	{
		child->_drawShape();
	}
}

void e2d::Node::_onEnter()
{
	if (!this->m_bDisplayedInScene)
	{
		this->m_bDisplayedInScene = true;
		this->onEnter();

		for (auto child : m_vChildren)
		{
			child->_onEnter();
		}
	}
}

void e2d::Node::_onExit()
{
	if (this->m_bDisplayedInScene)
	{
		this->m_bDisplayedInScene = false;
		this->onExit();

		for (auto child : m_vChildren)
		{
			child->_onExit();
		}
	}
}

void e2d::Node::_updateTransform()
{
	// �������ĵ�����
	D2D1_POINT_2F pivot = D2D1::Point2F(
		m_fWidth * m_fPivotX,
		m_fHeight * m_fPivotY
	);
	// �任 Initial �����ӽڵ㽫�������������б任
	m_MatriInitial = D2D1::Matrix3x2F::Scale(
		m_fScaleX,
		m_fScaleY,
		pivot
	) * D2D1::Matrix3x2F::Skew(
		m_fSkewAngleX,
		m_fSkewAngleY,
		pivot
	) * D2D1::Matrix3x2F::Rotation(
		m_fRotation,
		pivot
	) * D2D1::Matrix3x2F::Translation(
		m_fPosX,
		m_fPosY
	);
	// �����������ĵ�任 Final ����
	m_MatriFinal = m_MatriInitial * D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
	// �͸��ڵ�������
	if (m_pParent)
	{
		m_MatriInitial = m_MatriInitial * m_pParent->m_MatriInitial;
		m_MatriFinal = m_MatriFinal * m_pParent->m_MatriInitial;
	}
}

void e2d::Node::_updateChildrenTransform()
{
	for (auto child : m_vChildren)
	{
		_updateTransform(child);
	}
}

void e2d::Node::_updateTransform(Node * node)
{
	// ����������ת������
	node->_updateTransform();
	// ������������״Ҳ������Ӧת��
	if (node->m_pShape)
	{
		node->m_pShape->_transform();
	}
	// �����ӽڵ��µ����нڵ�
	node->_updateChildrenTransform();
	// ��־��ִ�й��任
	node->m_bTransformNeeded = false;
}

void e2d::Node::_updateChildrenOpacity()
{
	for (auto child : m_vChildren)
	{
		_updateOpacity(child);
	}
}

void e2d::Node::_updateOpacity(Node * node)
{
	if (node->m_pParent)
	{
		node->m_fDisplayOpacity = node->m_fRealOpacity * node->m_pParent->m_fDisplayOpacity;
	}
	node->_updateChildrenOpacity();
}

bool e2d::Node::isVisiable() const
{
	return m_bVisiable;
}

e2d::String e2d::Node::getName() const
{
	return m_sName;
}

double e2d::Node::getPosX() const
{
	return m_fPosX;
}

double e2d::Node::getPosY() const
{
	return m_fPosY;
}

e2d::Point e2d::Node::getPos() const
{
	return Point(m_fPosX, m_fPosY);
}

double e2d::Node::getWidth() const
{
	return m_fWidth * m_fScaleX;
}

double e2d::Node::getHeight() const
{
	return m_fHeight * m_fScaleY;
}

double e2d::Node::getRealWidth() const
{
	return m_fWidth;
}

double e2d::Node::getRealHeight() const
{
	return m_fHeight;
}

e2d::Size e2d::Node::getRealSize() const
{
	return Size(m_fWidth, m_fHeight);
}

double e2d::Node::getPivotX() const
{
	return m_fPivotX;
}

double e2d::Node::getPivotY() const
{
	return m_fPivotY;
}

e2d::Size e2d::Node::getSize() const
{
	return Size(getWidth(), getHeight());
}

double e2d::Node::getScaleX() const
{
	return m_fScaleX;
}

double e2d::Node::getScaleY() const
{
	return m_fScaleY;
}

double e2d::Node::getSkewX() const
{
	return m_fSkewAngleX;
}

double e2d::Node::getSkewY() const
{
	return m_fSkewAngleY;
}

double e2d::Node::getRotation() const
{
	return m_fRotation;
}

double e2d::Node::getOpacity() const
{
	return m_fRealOpacity;
}

e2d::Shape * e2d::Node::getShape() const
{
	return m_pShape;
}

int e2d::Node::getOrder() const
{
	return m_nOrder;
}

void e2d::Node::setOrder(int order)
{
	m_nOrder = order;
}

void e2d::Node::setPosX(double x)
{
	this->setPos(x, m_fPosY);
}

void e2d::Node::setPosY(double y)
{
	this->setPos(m_fPosX, y);
}

void e2d::Node::setPos(const Point & p)
{
	this->setPos(p.x, p.y);
}

void e2d::Node::setPos(double x, double y)
{
	if (m_fPosX == x && m_fPosY == y)
		return;

	m_fPosX = static_cast<float>(x);
	m_fPosY = static_cast<float>(y);
	m_bTransformNeeded = true;
}

void e2d::Node::movePosX(double x)
{
	this->movePos(x, 0);
}

void e2d::Node::movePosY(double y)
{
	this->movePos(0, y);
}

void e2d::Node::movePos(double x, double y)
{
	this->setPos(m_fPosX + x, m_fPosY + y);
}

void e2d::Node::movePos(const Vector & v)
{
	this->movePos(v.x, v.y);
}

void e2d::Node::_setSize(double width, double height)
{
	if (m_fWidth == width && m_fHeight == height)
		return;

	m_fWidth = static_cast<float>(width);
	m_fHeight = static_cast<float>(height);
	m_bTransformNeeded = true;
}

void e2d::Node::setScaleX(double scaleX)
{
	this->setScale(scaleX, m_fScaleY);
}

void e2d::Node::setScaleY(double scaleY)
{
	this->setScale(m_fScaleX, scaleY);
}

void e2d::Node::setScale(double scale)
{
	this->setScale(scale, scale);
}

void e2d::Node::setScale(double scaleX, double scaleY)
{
	if (m_fScaleX == scaleX && m_fScaleY == scaleY)
		return;

	m_fScaleX = static_cast<float>(scaleX);
	m_fScaleY = static_cast<float>(scaleY);
	m_bTransformNeeded = true;
}

void e2d::Node::setSkewX(double angleX)
{
	this->setSkew(angleX, m_fSkewAngleY);
}

void e2d::Node::setSkewY(double angleY)
{
	this->setSkew(m_fSkewAngleX, angleY);
}

void e2d::Node::setSkew(double angleX, double angleY)
{
	if (m_fSkewAngleX == angleX && m_fSkewAngleY == angleY)
		return;

	m_fSkewAngleX = static_cast<float>(angleX);
	m_fSkewAngleY = static_cast<float>(angleY);
	m_bTransformNeeded = true;
}

void e2d::Node::setRotation(double angle)
{
	if (m_fRotation == angle)
		return;

	m_fRotation = static_cast<float>(angle);
	m_bTransformNeeded = true;
}

void e2d::Node::setOpacity(double opacity)
{
	if (m_fRealOpacity == opacity)
		return;

	m_fDisplayOpacity = m_fRealOpacity = min(max(static_cast<float>(opacity), 0), 1);
	// ���½ڵ�͸����
	_updateOpacity(this);
}

void e2d::Node::setPivotX(double pivotX)
{
	this->setPivot(pivotX, m_fPivotY);
}

void e2d::Node::setPivotY(double pivotY)
{
	this->setPivot(m_fPivotX, pivotY);
}

void e2d::Node::setPivot(double pivotX, double pivotY)
{
	if (m_fPivotX == pivotX && m_fPivotY == pivotY)
		return;

	m_fPivotX = min(max(static_cast<float>(pivotX), 0), 1);
	m_fPivotY = min(max(static_cast<float>(pivotY), 0), 1);
	m_bTransformNeeded = true;
}

void e2d::Node::setShape(Shape * pShape)
{
	// ɾ���ɵ���״
	ShapeManager::__delShape(m_pShape);
	// �����µ���״
	ShapeManager::__addShape(pShape);

	if (pShape)
	{
		// ˫���
		this->m_pShape = pShape;
		pShape->m_pParentNode = this;
	}
	else
	{
		this->m_pShape = nullptr;
	}
}

void e2d::Node::addChild(Node * child, int order  /* = 0 */)
{
	WARN_IF(child == nullptr, "Node::addChild NULL pointer exception.");

	if (child)
	{
		ASSERT(child->m_pParent == nullptr, "Node already added. It can't be added again!");

		for (Node * parent = this; parent != nullptr; parent = parent->getParent())
		{
			ASSERT(child != parent, "A Node cannot be the child of his own children!");
		}

		m_vChildren.push_back(child);

		child->setOrder(order);

		child->retain();

		child->m_pParent = this;

		if (this->m_pParentScene)
		{
			child->_setParentScene(this->m_pParentScene);
		}

		if (this->m_bDisplayedInScene)
		{
			child->_onEnter();
		}

		// �����ӽڵ�͸����
		_updateOpacity(child);
		// ���½ڵ�ת��
		child->m_bTransformNeeded = true;
		// �����ӽڵ�����
		m_bSortChildrenNeeded = true;
	}
}

e2d::Node * e2d::Node::getParent() const
{
	return m_pParent;
}

e2d::Scene * e2d::Node::getParentScene() const
{
	return m_pParentScene;
}

std::vector<e2d::Node*>& e2d::Node::getChildren()
{
	return m_vChildren;
}

int e2d::Node::getChildrenCount() const
{
	return static_cast<int>(m_vChildren.size());
}

e2d::Node * e2d::Node::getChild(const String & name)
{
	WARN_IF(name.isEmpty(), "Invalid Node name.");

	unsigned int hash = name.getHash();

	for (auto child : m_vChildren)
	{
		// ��ͬ�����ƿ��ܻ�����ͬ�� Hash ֵ�������ȱȽ� Hash �������������ٶ�
		if (child->m_nHashName == hash && child->m_sName == name)
			return child;
	}
	return nullptr;
}

std::vector<e2d::Node*> e2d::Node::getChildren(const String & name)
{
	std::vector<Node*> vChildren;

	WARN_IF(name.isEmpty(), "Invalid Node name.");

	unsigned int hash = name.getHash();

	for (auto child : m_vChildren)
		if (child->m_nHashName == hash && child->m_sName == name)
			vChildren.push_back(child);

	return std::move(vChildren);
}

void e2d::Node::removeFromParent()
{
	if (m_pParent)
	{
		m_pParent->removeChild(this);
	}
}

bool e2d::Node::removeChild(Node * child)
{
	WARN_IF(child == nullptr, "Node::removeChildren NULL pointer exception.");

	if (m_vChildren.empty())
	{
		return false;
	}

	if (child)
	{
		size_t size = m_vChildren.size();
		for (size_t i = 0; i < size; i++)
		{
			if (m_vChildren[i] == child)
			{
				m_vChildren.erase(m_vChildren.begin() + i);
				child->m_pParent = nullptr;

				if (child->m_pParentScene)
				{
					child->_setParentScene(nullptr);
				}
				if (child->m_bDisplayedInScene)
				{
					child->_onExit();
				}

				child->release();
				return true;
			}
		}
	}
	return false;
}

void e2d::Node::removeChildren(const String & childName)
{
	WARN_IF(childName.isEmpty(), "Invalid Node name.");

	if (m_vChildren.empty())
	{
		return;
	}

	// �������� Hash ֵ
	unsigned int hash = childName.getHash();

	size_t size = m_vChildren.size();
	for (size_t i = 0; i < size; i++)
	{
		auto child = m_vChildren[i];
		if (child->m_nHashName == hash && child->m_sName == childName)
		{
			m_vChildren.erase(m_vChildren.begin() + i);
			child->m_pParent = nullptr;
			if (child->m_pParentScene)
			{
				child->_setParentScene(nullptr);
			}
			if (child->m_bDisplayedInScene)
			{
				child->_onExit();
			}
			child->release();
		}
	}
}

void e2d::Node::clearAllChildren()
{
	// ���нڵ�����ü�����һ
	for (auto child : m_vChildren)
	{
		if (child->m_bDisplayedInScene)
		{
			child->_onExit();
		}
		child->release();
	}
	// ��մ���ڵ������
	m_vChildren.clear();
}

void e2d::Node::runAction(Action * action)
{
	if (this != action->getTarget())
	{
		WARN_IF(
			nullptr != action->getTarget(),
			"The action has already got a target, The clone of the action will be created automatically!"
		);

		if (nullptr != action->getTarget())
		{
			action = action->clone();
		}
		ActionManager::_add(action, this);
	}
	else
	{
		action->reset();
	}
}

void e2d::Node::resumeAction(Action * action)
{
	if (action->getTarget() == this)
	{
		action->resume();
	}
}

void e2d::Node::pauseAction(Action * action)
{
	if (action->getTarget() == this)
	{
		action->pause();
	}
}

bool e2d::Node::isPointIn(Point point)
{
	if (m_bTransformNeeded)
	{
		_updateTransform(this);
	}
	// Ϊ�ڵ㴴��һ����״
	ID2D1RectangleGeometry * rect;
	Renderer::getID2D1Factory()->CreateRectangleGeometry(
		D2D1::RectF(0, 0, m_fWidth * m_fScaleX, m_fHeight * m_fScaleY),
		&rect
	);
	// �жϵ��Ƿ�����״��
	BOOL ret;
	rect->FillContainsPoint(
		D2D1::Point2F(
			static_cast<float>(point.x), 
			static_cast<float>(point.y)),
		&m_MatriFinal,
		&ret
	);
	if (ret)
	{
		return true;
	}
	else
	{
		for (auto child : m_vChildren)
			if (child->isPointIn(point))
				return true;
	}
	return false;
}

void e2d::Node::setAutoUpdate(bool bAutoUpdate)
{
	m_bAutoUpdate = bAutoUpdate;
}

void e2d::Node::setDefaultPiovt(double defaultPiovtX, double defaultPiovtY)
{
	s_fDefaultPiovtX = min(max(static_cast<float>(defaultPiovtX), 0), 1);
	s_fDefaultPiovtY = min(max(static_cast<float>(defaultPiovtY), 0), 1);
}

void e2d::Node::stopAction(Action * action)
{
	if (action->getTarget() == this)
	{
		action->stop();
	}
}

void e2d::Node::resumeAllActions()
{
	ActionManager::resumeAllActionsBindedWith(this);
}

void e2d::Node::pauseAllActions()
{
	ActionManager::pauseAllActionsBindedWith(this);
}

void e2d::Node::stopAllActions()
{
	ActionManager::stopAllActionsBindedWith(this);
}

void e2d::Node::setVisiable(bool value)
{
	m_bVisiable = value;
}

void e2d::Node::setName(const String & name)
{
	WARN_IF(name.isEmpty(), "Invalid Node name.");

	if (!name.isEmpty() && m_sName != name)
	{
		// ����ڵ���
		m_sName = name;
		// ����ڵ� Hash ��
		m_nHashName = name.getHash();
	}
}

void e2d::Node::_setParentScene(Scene * scene)
{
	m_pParentScene = scene;
	for (auto child : m_vChildren)
	{
		child->_setParentScene(scene);
	}
}