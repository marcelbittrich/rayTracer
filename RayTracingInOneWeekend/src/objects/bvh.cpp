#include "bvh.h"

#include "hittablelist.h"

BVH_Node::BVH_Node(Scene* list)
	: BVH_Node(list->GetObjectPointers(), 0, list->GetObjectPointers().size())
{
}
