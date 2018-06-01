#pragma once
#include"color.h"
#include"vector3.h"

class Material {
public:
	Color color, absor;
	float refl, refr;
	float diff;
	float rindex;

	Material(Color _color = Color(0.5f, 0.5f, 0.5f), Color _absor = Color(0.2f, 0.2f, 0.2f),
		float _refl = 0.5f, float _refr = 0.5f, float _diff = 0.f, float _rindex = 1.6f) :
		color(_color), absor(_absor),
		refl(_refl), refr(_refr), diff(_diff), rindex(_rindex) {}
	~Material() {}

};

struct Crash
{
	bool crashed;
	Vector3 position;
	Vector3 normal;
	float dist;
	bool front;
};

class Primitive
{
protected:
	Material material;
public:
	Primitive() {};
	virtual ~Primitive() {};

	Material& GetMaterial() { return material; }
	virtual Crash Collide(Vector3& origin, Vector3 direction) = 0;  //�������ʵ��Collide
};

class Sphere : public Primitive
{
	Vector3 center;
	float radius;

public:
	Sphere(Vector3 _center,float _radius) :Primitive(),center(_center),radius(_radius) {};
	~Sphere() {}
	Crash Collide(Vector3& origin, Vector3& direction);
};