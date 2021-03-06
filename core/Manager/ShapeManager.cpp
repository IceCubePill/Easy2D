#include "..\emanagers.h"
#include "..\enodes.h"
#include "..\eshape.h"

// 形状集合
static std::vector<e2d::Shape*> s_vShapes;
// 碰撞触发状态
static bool s_bCollisionEnable = false;


void e2d::ShapeManager::setCollisionEnable(bool bEnable)
{
	s_bCollisionEnable = bEnable;
}

void e2d::ShapeManager::__updateShape(e2d::Shape * pActiveShape)
{
	// 判断碰撞触发是否打开
	if (!s_bCollisionEnable)
		return;

	Node* pActiveNode = pActiveShape->m_pParentNode;
	if (pActiveNode)
	{
		// 获取节点所在场景
		Scene* pCurrentScene = pActiveNode->getParentScene();
		
		// 判断与其他形状的交集情况
		for (size_t i = 0; i < s_vShapes.size(); i++)
		{
			auto pPassiveShape = s_vShapes[i];
			// 判断两个形状是否是同一个对象
			if (pActiveShape == pPassiveShape)
				return;
			// 判断两物体是否是相互冲突的物体
			if (pActiveShape->m_nCollisionBitmask & pPassiveShape->m_nCategoryBitmask)
			{
				// 获取被碰撞节点
				Node* pPassiveNode = pPassiveShape->m_pParentNode;
				// 判断两节点是否处于同一场景中
				if (pPassiveNode &&
					pPassiveNode->getParentScene() == pCurrentScene)
				{
					// 判断两形状交集情况
					int relation = pActiveShape->getRelationWith(pPassiveShape);
					// 忽略 UNKNOWN 和 DISJOINT 情况
					if (relation != Relation::UNKNOWN && relation != Relation::DISJOINT)
					{
						pActiveNode->onCollide(pPassiveNode, relation);
						pPassiveNode->onCollide(pActiveNode, pPassiveShape->getRelationWith(pActiveShape));
						pCurrentScene->onCollide(pActiveNode, pPassiveNode);
					}
				}
			}
		}
	}
}

void e2d::ShapeManager::__add(Shape * pShape)
{
	if (pShape)
	{
		if (pShape->m_pParentNode)
		{
			WARN_IF(true, "ShapeManager::__add Failed! The shape is already added.");
			return;
		}
		pShape->retain();
		s_vShapes.push_back(pShape);
	}
}

void e2d::ShapeManager::__remove(Shape * pShape)
{
	if (pShape)
	{
		for (size_t i = 0; i < s_vShapes.size(); i++)
		{
			if (s_vShapes[i] == pShape)
			{
				SafeRelease(&pShape);
				s_vShapes.erase(s_vShapes.begin() + i);
				return;
			}
		}
	}
}
