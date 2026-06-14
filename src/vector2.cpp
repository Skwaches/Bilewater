
#include "vector2.h"

Vector2::Vector2(float x, float y):x(x),y(y){};
Vector2::Vector2(const float* array):x(array[0]),y(array[1]){};
Vector2::Vector2(SDL_FPoint point):x(point.x),y(point.y){};

float Vector2::magnitude(){
	return SDL_sqrtf(x*x + y*y);
}

Vector2& Vector2::operator+=(const Vector2& other){
	x += other.x;
	y += other.y;
	return *this;
}
Vector2& Vector2::operator+=(const float& other){
	x += other;
	y += other;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& other){
	x -= other.x;
	y -= other.y;
	return *this;

}
Vector2& Vector2::operator-=(const float& other){
	x -= other;
	y -= other;
	return *this;
	
}

Vector2& Vector2::operator*=(const Vector2& other){
	x *= other.x;
	y *= other.y;
	return *this;
}
Vector2& Vector2::operator*=(const float& other){
	x *= other;
	y *= other;
	return *this;
}

Vector2& Vector2::operator/=(const Vector2& other){
	x /= other.x;
	y /= other.y;
	return *this;
}
Vector2& Vector2::operator/=(const float& other){
	x /= other;
	y /= other;
	return *this;
}

Vector2 Vector2::operator+(const Vector2& other) const{
	Vector2 result = *this;
	result+=other;
	return result;
}
Vector2 Vector2::operator+(const float& other)const{
	Vector2 result = *this;
	result+=other;
	return result;
}

Vector2 Vector2::operator-(const Vector2& other) const{
	Vector2 result = *this;
	result-=other;
	return result;
}
Vector2 Vector2::operator-(const float& other)const{
	Vector2 result = *this;
	result-=other;
	return result;
}

Vector2 Vector2::operator*(const Vector2& other) const{
	Vector2 result = *this;
	result*=other;
	return result;
}
Vector2 Vector2::operator*(const float& other) const{
	Vector2 result = *this;
	result*=other;
	return result;
}
Vector2 Vector2::operator/(const Vector2& other) const{
	Vector2 result = *this;
	result/=other;
	return result;
}
Vector2 Vector2::operator/(const float& other) const{
	Vector2 result = *this;
	result/=other;
	return result;
}
