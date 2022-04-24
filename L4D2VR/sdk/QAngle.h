#pragma once

class QAngleByValue;
class Vector;

typedef float vec_t;

class QAngle
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction
	QAngle();
	QAngle(vec_t X, vec_t Y, vec_t Z);
	//	QAngle(RadianEuler const &angles);	// evil auto type promotion!!!

	// Allow pass-by-value
	operator QAngleByValue &() { return *((QAngleByValue *)(this)); }
	operator const QAngleByValue &() const { return *((const QAngleByValue *)(this)); }

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f);
	void Random(vec_t minVal, vec_t maxVal);

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t &operator[](int i);

	// Base address...
	vec_t *Base();
	vec_t const *Base() const;

	// equality
	bool operator==(const QAngle &v) const;
	bool operator!=(const QAngle &v) const;

	// arithmetic operations
	QAngle &operator+=(const QAngle &v);
	QAngle &operator-=(const QAngle &v);
	QAngle &operator*=(float s);
	QAngle &operator/=(float s);

	// Get the vector's magnitude.
	vec_t	Length() const;
	vec_t	LengthSqr() const;

	// negate the QAngle components
	//void	Negate(); 

	// No assignment operators either...
	QAngle &operator=(const QAngle &src);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors

	// arithmetic operations
	QAngle	operator-(void) const;

	QAngle	operator+(const QAngle &v) const;
	QAngle	operator-(const QAngle &v) const;
	QAngle	operator*(float fl) const;
	QAngle	operator/(float fl) const;
#else

private:
	// No copy constructors allowed if we're in optimal mode
	QAngle(const QAngle &vOther);

#endif

	static void AngleVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up);
	static void VectorAngles(const Vector &forward, QAngle &angles);
	static void VectorAngles(const Vector &forward, const Vector &pseudoup, QAngle &angles);
};



