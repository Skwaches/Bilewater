#pragma once
#include <SDL3/SDL.h>
class Vector2{
	public:
		float x,y;
		Vector2(float x = 0, float y = 0);
		Vector2(const float* array);
		Vector2(SDL_FPoint point);
		float magnitude ();

		operator SDL_FPoint() const{
			return {x,y};
		}

		Vector2& operator+=(const Vector2& other);
		Vector2& operator+=(const float& other);

		Vector2& operator-=(const Vector2& other);
		Vector2& operator-=(const float& other);

		Vector2& operator*=(const Vector2& other);
		Vector2& operator*=(const float& other);

		Vector2& operator/=(const Vector2& other);
		Vector2& operator/=(const float& other);

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator+(const float& other)const;

		Vector2 operator-(const Vector2& other) const;
		Vector2 operator-(const float& other)const;

		Vector2 operator*(const Vector2& other) const;
		Vector2 operator*(const float& other) const;

		Vector2 operator/(const Vector2& other) const;
		Vector2 operator/(const float& other) const;
};
