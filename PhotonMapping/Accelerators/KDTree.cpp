#include "KDTree.h"
#include <algorithm>
//包围盒
AABB KDTree::GetBoundingBox(const vector<Primitive*>& prims) const
{
	AABB res = prims[0]->GetAABB();
	for (unsigned int i = 1; i < prims.size(); ++i)
		res.Extend(prims[i]->GetAABB());
	return res;
}

//prims为物体vector的指针的原因是在堆上分配内存防止stack overflow
//若为vector<Primitive> &prims可能溢出
KDNode* KDTree::BuildTree(vector<Primitive*>* prims, int depth)
{	
	if (depth > height)
		height = depth;
	//拷贝赋值
	KDNode* node = new KDNode(*prims);
	//包围盒
	node->aabb = GetBoundingBox(*prims);
	if (prims->size() == 1)
		return node;
	Vector3 midPoint = Vector3(0, 0, 0);
	//以三角形重心为划分标准,小于在左，大于在右
	for (auto prim : *prims)
		midPoint += (prim->GetBarycentre()/ double(prims->size()));
	auto left_prims=new vector<Primitive*>();
	auto right_prims= new vector<Primitive*>();
	int splitAxis = node->aabb.GetLongestAxis();
	for (auto prim :*prims)
	{
		if (prim->GetBarycentre().GetCoord(splitAxis) < midPoint.GetCoord(splitAxis))
			left_prims->push_back(prim);
		else
			right_prims->push_back(prim);
	}
	//若平均重心无法分离，子节点直接构造
	if (left_prims->size() == 0)
		node->right = new KDNode(*right_prims,GetBoundingBox(*right_prims));
	else if (right_prims->size() == 0)
		node->left = new KDNode(*left_prims, GetBoundingBox(*left_prims));
	else
	{
		//否则继续划分
		node->left = BuildTree(left_prims, depth + 1);
		node->right = BuildTree(right_prims, depth + 1);
	}
	delete left_prims;
	delete right_prims;
	return node;
}
void KDTree::BuildTree(vector<Primitive*>* prims)
{
	makeEmpty(root);
	root = BuildTree(prims, 0);
}
void KDTree::Collide(KDNode* node,Ray& ray, Crash& crashResult) const
{
	if (node->aabb.Hit(ray))
	{
		//到达叶节点
		if (node->left == nullptr&&node->right == nullptr)
		{
			//对叶节点的物体遍历求交
			for (auto prim : node->primitives)
			{
				Crash curCrash = prim->Collide(ray);
				//光线可能与多个三角片相交，只取距离最短的
				if (curCrash.crashed&&curCrash.dist < crashResult.dist)
					crashResult = curCrash;
			}
		}
		else
		{
			if (node->left != nullptr)
				Collide(node->left, ray, crashResult);
			if (node->right != nullptr)
				Collide(node->right, ray, crashResult);
		}
	}
}
void KDTree::Collide(Ray& ray, Crash& crashResult) const
{
	Collide(root, ray, crashResult);
}
void KDTree::makeEmpty(KDNode* &t)
{
	if (t != nullptr)
	{
		makeEmpty(t->left);
		makeEmpty(t->right);
		delete t;
	}
	t = nullptr;
}
KDTree::~KDTree()
{
	makeEmpty(root);
}