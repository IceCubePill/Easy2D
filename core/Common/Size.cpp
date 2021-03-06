#include "..\ecommon.h"

e2d::Size::Size()
{
	width = 0;
	height = 0;
}

e2d::Size::Size(double width, double height)
{
	this->width = width;
	this->height = height;
}

e2d::Size e2d::Size::operator+(Size const & size)
{
	return Size(width + size.width, height + size.height);
}

e2d::Size e2d::Size::operator-(Size const & size)
{
	return Size(width - size.width, height - size.height);
}

e2d::Size e2d::Size::operator*(double const & value)
{
	return Size(width * value, height * value);
}

e2d::Size e2d::Size::operator/(double const & value)
{
	return Size(width / value, height / value);
}

e2d::Size::operator e2d::Point() const
{
	return Point(width, height);
}
