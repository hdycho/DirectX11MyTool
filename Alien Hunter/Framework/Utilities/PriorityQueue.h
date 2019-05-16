#pragma once

class IPriorityNode
{
public:
	virtual float Priority() = 0;
	virtual void Priority(float val) = 0;

	virtual int InsertionIndex() = 0;
	virtual void InsertionIndex(int val) = 0;

	virtual int QueueIndex() = 0;
	virtual void QueueIndex(int val) = 0;
};

class PriorityQueue
{
public:
	PriorityQueue(int maxCount);
	~PriorityQueue();


	void Enqueue(IPriorityNode* node, float priority);
	IPriorityNode* Dequeue();

	IPriorityNode* First() { return nodes[1]; }

	void Remove(IPriorityNode* node);

	bool Contain(IPriorityNode* node) { return nodes[node->QueueIndex()] == node; }
	bool Empty() { return count <= 1; }

private:
	void NodeUpdate(IPriorityNode* node);
	bool HigherPriority(IPriorityNode* higher, IPriorityNode* lower);
	void CascadeUp(IPriorityNode* node);
	void CascadeDown(IPriorityNode* node);

	void Swap(IPriorityNode* node1, IPriorityNode* node2);
private:
	vector<IPriorityNode*> nodes;
	int nodeEverEnqueueCount;

	int count;
};
