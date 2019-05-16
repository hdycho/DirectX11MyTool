#pragma once

class ICollider
{
public:
	virtual void CreateCollider(int type)=0;
	virtual void ColliderUpdate()=0;
	virtual vector<ColliderElement*>&GetColliders() = 0;

};
