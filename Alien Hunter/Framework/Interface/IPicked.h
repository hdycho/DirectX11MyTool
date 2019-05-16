#pragma once

class IPicked
{
public:
	virtual bool IsPicked() = 0;
	virtual void SetPickState(bool val) = 0;
};
