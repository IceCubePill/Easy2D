#include "..\easy2d.h"

RectNode::RectNode() :
	m_Rect(0, 0, 0, 0)
{

}

RectNode::~RectNode()
{
}

bool RectNode::isCollisionWith(RectNode * rectNode) const
{
	static CRect rt;
	return IntersectRect(&rt, &m_Rect, &rectNode->m_Rect);
}

bool RectNode::isPointIn(CPoint p) const
{
	return m_Rect.PtInRect(p);
}

void RectNode::setWindowCenterX()
{
	setX((App::getWidth() - getWidth()) / 2);
}

void RectNode::setWindowCenterY()
{
	setY((App::getHeight() - getHeight()) / 2);
}

void RectNode::setWindowCenter()
{
	setWindowCenterX();
	setWindowCenterY();
}

int RectNode::getX() const
{
	return m_Rect.left;
}

int RectNode::getY() const
{
	return m_Rect.top;
}

CPoint RectNode::getPos() const
{
	return m_Rect.TopLeft();
}

int RectNode::getWidth() const
{
	return m_Rect.Width();		// 矩形宽度
}

int RectNode::getHeight() const
{
	return m_Rect.Height();		// 矩形高度
}

CSize RectNode::getSize() const
{
	return m_Rect.Size();
}

CRect RectNode::getRect() const
{
	return m_Rect;
}

void RectNode::setX(int x)
{
	if (m_pParent)
	{
		m_Rect.MoveToX(m_pParent->getX() + x);
	}
	else
	{
		m_Rect.MoveToX(x);
	}
}

void RectNode::setY(int y)
{
	if (m_pParent)
	{
		m_Rect.MoveToY(m_pParent->getY() + y);
	}
	else
	{
		m_Rect.MoveToY(y);
	}
}

void RectNode::setPos(int x, int y)
{
	if (m_pParent)
	{
		m_Rect.MoveToXY(m_pParent->getX() + x, m_pParent->getY() + y);
	}
	else
	{
		m_Rect.MoveToXY(x, y);
	}
}

void RectNode::setPos(CPoint p)
{
	if (m_pParent)
	{
		m_Rect.MoveToXY(m_pParent->getPos() + p);
	}
	else
	{
		m_Rect.MoveToXY(p);
	}
}

void RectNode::move(int x, int y)
{
	m_Rect.OffsetRect(x, y);	// 移动矩形
}

void RectNode::move(CVector v)
{
	m_Rect.OffsetRect(v);		// 移动矩形
}

void RectNode::setWidth(int width)
{
	m_Rect.right = max(m_Rect.left + width, 0);
}

void RectNode::setHeight(int height)
{
	m_Rect.bottom = max(m_Rect.top + height, 0);
}

void RectNode::setSize(int width, int height)
{
	setWidth(width);
	setHeight(height);
}

void RectNode::setSize(CSize size)
{
	setSize(size.cx, size.cy);
}

void RectNode::setRect(int x1, int y1, int x2, int y2)
{
	m_Rect.SetRect(x1, y1, x2, y2);
}

void RectNode::setRect(CPoint leftTop, CPoint rightBottom)
{
	m_Rect.SetRect(leftTop, rightBottom);
}

void RectNode::setRect(CRect rect)
{
	m_Rect.CopyRect(&rect);
}
