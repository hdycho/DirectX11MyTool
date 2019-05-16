#pragma once

#include <string>
#pragma warning( disable : 4996)

//////////////////////////////////////////////////////////////////////////
///@brief Æ÷ÀÎÆ®
//////////////////////////////////////////////////////////////////////////
class Point
{
public:
	Point(void);
	Point(int x, int y);

	bool operator ==(const Point& value2) const;
	bool operator !=(const Point& value2) const;
	
	std::wstring ToString();

public:
	const static Point Zero;///< 0, 0

	int X;///< X
	int Y;///< Y
};