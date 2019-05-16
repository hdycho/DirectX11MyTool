#pragma once

#include <string>
#pragma warning( disable : 4996)

class Point;

//////////////////////////////////////////////////////////////////////////
///@brief ¿µ¿ª
//////////////////////////////////////////////////////////////////////////
class Rect
{
public:
	Rect(void);
	Rect(int x, int y, int width, int height);

	bool operator ==(const Rect& value) const;
	bool operator !=(const Rect& value) const;

	std::wstring ToString();

	int Left() const;
	int Right() const;
	int Top() const;
	int Bottom() const;
	
	Point Center() const;
	Point Location() const;
	void Location(Point value);

	bool IsEmpty() const;

	void Offset(Point amount);
	void Offset(int offsetX, int offsetY);

	void Inflate(int horizontalAmount, int verticalAmount);

	bool Contains(int x, int y);
	bool Contains(Point value);
	bool Contains(Rect value);

	bool Intersects(const Rect& value);
	static Rect Intersect(const Rect& value1, const Rect& value2);
	
	static Rect Union(Rect value1, Rect value2);

public:
	const static Rect Empty;

	int X;///< X
	int Y;///< Y
	int Width;///< WIdth
	int Height;///< Height
};