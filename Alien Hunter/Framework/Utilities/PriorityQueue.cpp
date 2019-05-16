#include "Framework.h"
#include "PriorityQueue.h"

PriorityQueue::PriorityQueue(int maxCount)
{
	count = 0;
	nodeEverEnqueueCount = 0;

	nodes.assign(maxCount + 1, NULL);
}

PriorityQueue::~PriorityQueue()
{
	
}

void PriorityQueue::Remove(IPriorityNode * node)
{
	if (count <= 1)
	{
		nodes[1] = NULL;
		count = 0;
		return;
	}

	bool wasSwaped = false;
	IPriorityNode* lastNode = nodes[count];
	if (node->QueueIndex() != count)
	{
		Swap(node, lastNode);
		wasSwaped = true;
	}

	count--;
	nodes[node->QueueIndex()] = NULL;

	if (wasSwaped == true)
		NodeUpdate(lastNode);
}

void PriorityQueue::Swap(IPriorityNode * node1, IPriorityNode * node2)
{
	nodes[node1->QueueIndex()] = node2;
	nodes[node2->QueueIndex()] = node1;

	int temp = node1->QueueIndex();
	node1->QueueIndex(node2->QueueIndex());
	node2->QueueIndex(temp);
}

void PriorityQueue::Enqueue(IPriorityNode * node, float priority)
{
	node->Priority(priority);
	count++;

	nodes[count] = node;
	node->QueueIndex(count);
	node->InsertionIndex(nodeEverEnqueueCount++);

	CascadeUp(nodes[count]);
}

IPriorityNode * PriorityQueue::Dequeue()
{
	IPriorityNode* node = nodes[1];
	Remove(node);

	return node;
}

void PriorityQueue::NodeUpdate(IPriorityNode * node)
{
	int parentIndex = node->QueueIndex() / 2;
	IPriorityNode* parentNode = nodes[parentIndex];

	if (parentIndex > 0 && HigherPriority(node, parentNode))
		CascadeUp(node);
	else
		CascadeDown(node);
}

bool PriorityQueue::HigherPriority(IPriorityNode * higher, IPriorityNode * lower)
{
	bool b = false;
	b |= fabsf(higher->Priority() - lower->Priority()) < Math::EPSILON;
	b &= higher->InsertionIndex() < lower->InsertionIndex();
	b |= higher->Priority() < lower->Priority();

	return b;
}

void PriorityQueue::CascadeUp(IPriorityNode * node)
{
	int parent = node->QueueIndex() / 2;
	while (parent >= 1) 
	{
		IPriorityNode* parentNode = nodes[parent];
		if (HigherPriority(parentNode, node))
			break;

		Swap(node, parentNode);
		parent = node->QueueIndex() / 2;
	}
}

void PriorityQueue::CascadeDown(IPriorityNode * node)
{
	int finalQueueIndex = node->QueueIndex();
	while (true)
	{
		IPriorityNode* newParent = node;
		int childLeftIndex = 2 * finalQueueIndex;

		if (childLeftIndex > count)
		{
			node->QueueIndex(finalQueueIndex);
			nodes[finalQueueIndex] = node;

			break;
		}

		IPriorityNode* childLeft = nodes[childLeftIndex];
		if (HigherPriority(childLeft, newParent))
			newParent = childLeft;

		int childRightIndex = childLeftIndex + 1;
		if (childRightIndex <= count)
		{
			IPriorityNode* childRight = nodes[childRightIndex];
			if (HigherPriority(childRight, newParent))
				newParent = childRight;
		}
		if (newParent != node)
		{
			nodes[finalQueueIndex] = newParent;
			int temp = newParent->QueueIndex();
			newParent->QueueIndex(finalQueueIndex);
			finalQueueIndex = temp;
		}
		else
		{
			node->QueueIndex(finalQueueIndex);
			nodes[finalQueueIndex] = node;

			break;
		}
	}
}
