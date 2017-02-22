#include "mesh.hpp"
#include <iostream>
#include <assert.h>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}

bool Mesh::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal)
{
	for (std::vector<Mesh::Face>::const_iterator I = m_faces.begin(); 
			I != m_faces.end(); ++I) {
    
		Point3D* corePoint = &m_verts.at(*(I->begin()));
		Point3D* previousPoint = NULL;
		for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
			int vertex = *J;
			Point3D point = m_verts.at(vertex);

			if(previousPoint != NULL){
				Point3D intersection;

				bool hit = intersectTriangle(*corePoint, *previousPoint, point, 
										rayOrigin, ray, intersection, normal);

				if(hit){
					t = (intersection - rayOrigin).length() / ray.length();
					if(t < 0)
						return false;
	
					return true;
				}

			}
			
			previousPoint = new Point3D(point);
		}
	}

	return false;
}

bool Mesh::intersectTriangle(Point3D a, Point3D b, Point3D c, 
			Point3D origin, Vector3D ray, Point3D &intersection, Vector3D &normal)
{
	Vector3D u = b-a;
	Vector3D v = c-a; 	

	Vector3D w = origin - a;

	normal = u.cross(v);

	double s = (u.dot(v)) * (w.dot(u)) - (v.dot(v)) * (w.dot(v));
	s = s / ((u.dot(v)) * (u.dot(v)) - (u.dot(u)) * (v.dot(v)));

	double t = (u.dot(v)) * (w.dot(u)) - (u.dot(u)) * (w.dot(v));
	t = t / ((u.dot(v)) * (u.dot(v)) - (u.dot(u)) * (v.dot(v)));

	if(s + t > 1)
	{
		return false;
	}

	intersection = a + s*u + t*v;	
	return true;
}

