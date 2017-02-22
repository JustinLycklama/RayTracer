#include "primitive.hpp"
#include "polyroots.hpp"

Primitive::~Primitive()
{
}

bool Primitive::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal)
{
	return false;
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}


NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal)
{
			double roots[2];

			//std::cerr<<"checkcollide";

			Vector3D origin_center = Vector3D(rayOrigin[0] - m_pos[0], 
																				rayOrigin[1] - m_pos[1],
																				rayOrigin[2] - m_pos[2]);

			double A, B , C;
			A = ray.dot(ray);
			B = 2*(origin_center).dot(ray);
			C = origin_center.dot(origin_center) - (m_radius * m_radius);

			size_t size = quadraticRoots(A, B, C, roots);

			if(size == 0)
			{
				return false;
			}
			else if(size == 1){
				t = roots[0];
			}
			else if(size == 2){
				if(roots[0] < roots[1])
					t = roots[0];
				else
					t = roots[1];

				if(t < 0){
					if(roots[1] > 0)
					t = roots[1];
				else
					t = roots[0];
				}
			}

			if(t < 0){
				return false;
			}
			
			Point3D intersection = rayOrigin + t*ray;
			normal = intersection - m_pos;

			return true;	
}

NonhierBox::~NonhierBox()
{
}

/*bool NonhierSphere::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal)
{
	// Check intersection on all 6 faces

	Vector3D persistantNormal;
	t = 100000;

	{
	// FRONT
	Point3D a = Point3D(m_pos[0] - m_size/3, m_pos[1] - m_size / 3, m_pos[2] - m_size/2);
	Point3D b = Point3D(m_pos[0] + m_size/3, m_pos[1] - m_size / 3, m_pos[2] - m_size/2);
	Point3D c = Point3D(m_pos[0], m_pos[1], m_pos[2] - m_size/2);

	Vector3D d = a - b;
	Vector3D e = a - c;

	Vector3D normal = d.cross(e);
	Vector3D w = rayOrigin - c;

	double s = -1 * (normal.dot(w)) / (normal.dot(ray));
	
	Point3D point = rayOrigin + s * ray;

	if(){
		if(s > 0 && s < t)
		{
			t = s;
			persistantNormal = normal;
		}
	}
	}
	
	// BACK
	Point3D c = Point3D(m_pos[0] - m_size/3, m_pos[1] - m_size / 3, m_pos[2] + m_size/2);
	Point3D d = Point3D(m_pos[0] + m_size/3, m_pos[1] + m_size / 3, m_pos[2] + m_size/2);
		
	// LEFT
	Point3D e = Point3D(m_pos[0] - m_size/2, m_pos[1] - m_size / 3, m_pos[2] - m_size/3);
	Point3D f = Point3D(m_pos[0] - m_size/2, m_pos[1] + m_size / 3, m_pos[2] + m_size/3);
	
	// RIGHT
	Point3D g = Point3D(m_pos[0] + m_size/2, m_pos[1] - m_size / 3, m_pos[2] - m_size/3);
	Point3D h = Point3D(m_pos[0] + m_size/2, m_pos[1] + m_size / 3, m_pos[2] + m_size/3);
		
	// TOP
	Point3D i = Point3D(m_pos[0] - m_size/3, m_pos[1] - m_size / 2, m_pos[2] - m_size/3);
	Point3D j = Point3D(m_pos[0] + m_size/3, m_pos[1] - m_size / 2, m_pos[2] + m_size/3);
	
	// BOTTOM
	Point3D k = Point3D(m_pos[0] - m_size/3, m_pos[1] + m_size / 2, m_pos[2] - m_size/3);
	Point3D l = Point3D(m_pos[0] + m_size/3, m_pos[1] + m_size / 2, m_pos[2] + m_size/3);
		
		



	Vector3D w = rayOrigin	
	
}*/
