#include "stdafx.h"
#include "DebugObj.h"

DebugObj::DebugObj(UINT vCount, UINT iCount)
	:vertexCount(vCount),indexCount(iCount)
{
}

DebugObj::~DebugObj()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	
	SAFE_DELETE(material);
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(vertices);
	SAFE_DELETE(indices);
}
