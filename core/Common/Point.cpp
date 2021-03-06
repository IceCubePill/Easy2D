#include "..\ecommon.h"


e2d::Point::Point()
{
	x = 0;
	y = 0;
}

e2d::Point::Point(double x, double y)
{
	this->x = x;
	this->y = y;
}

e2d::Point e2d::Point::operator+(Point const & p)
{
	return Point(x + p.x, y + p.y);
}

e2d::Point e2d::Point::operator-(Point const & p)
{
	return Point(x - p.x, y - p.y);
}

e2d::Point e2d::Point::operator*(double const & value)
{
	return Point(x * value, y * value);
}

e2d::Point e2d::Point::operator/(double const & value)
{
	return Point(x / value, y / value);
}

e2d::Point::operator e2d::Size() const
{
	return Size(x, y);
}
