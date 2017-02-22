#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;
 
	bool checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal);

	bool intersectTriangle(Point3D a, Point3D b, Point3D c, 
			Point3D origin, Vector3D ray, Point3D &intersection, Vector3D &normal);

private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
