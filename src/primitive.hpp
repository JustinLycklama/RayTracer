#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
	virtual ~Primitive();	
	//virtual void getSize(Point3D &m_pos, double m_size);
	//virtual Ptype getType();

	bool checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal);
};

class Sphere : public Primitive {
public:

  virtual ~Sphere();	
};

class Cube : public Primitive {
public:

  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius) 
  {
  }

  virtual ~NonhierSphere();
	bool checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal);

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size) 
  {
  }
  
  virtual ~NonhierBox();

private:
  Point3D m_pos;
  double m_size;
};

#endif
