#include "mesh.hpp"
#include <iostream>
#include <assert.h>

using namespace std;

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
}

// Print a description of the mesh
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

// Refer to the documentation for Triangle Intersection
bool Mesh::intersectTriangle(Point3D v0, Point3D v1, Point3D v2, 
			Point3D origin, Vector3D ray, Point3D &intersection, Vector3D &normal)
{
    // Precalculate required vectors

    // Define plane normal using triangle edegs.
    Vector3D u = v1 - v0;       // Triangle edge one
    Vector3D v = v2 - v0;       // Triangle edge two
    
    // The cross product between two vectors (defining a plane) results in the normal of that plane
    normal = u.cross(v);

    // If the three points do not form a triangle but a line segment, we will be left with this
    if (normal[0] == 0 && normal[1] == 0 && normal[2] == 0) { 
        return false;
    }
    
    // Line - Plane Intersection

    Vector3D planeToRay = origin - v0;
    float numerator     = -1 * normal.dot(planeToRay);
    float denominator   = normal.dot(ray);              // If this is zero, the ray is parallel the plane

    // The ray is  parallel (or close enough) to the triangle plane
    if (fabs(denominator) < 0.00001) {     
        return false;
    }

    // Intersect point of ray with triangle plane
    float r = numerator / denominator;

    // Intersection of the line with the plane is on the other side of the origin point 
    if (r < 0.0) {
        return false;
    }

     // intersect point of ray and plane
    intersection = origin + r*ray;

    // Ray - Triangle Intersection

    //Is the intersection point inside the triangle?
    float    uu, uv, vv, wu, wv, D;

    Vector3D w = intersection - v0; // Intersection vector

    uu = u.dot(u);
    uv = u.dot(v);
    vv = v.dot(v);
    wu = w.dot(u);
    wv = w.dot(v);
    
    D = uv * uv - uu * vv;

    // Calculate s and t. As long as the following holds, our intersection point is in the triangle
    // 0 <= s <= 1, 0 <= t <= 1, s + t <= 1
    float s, t;
    
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0){         // I is outside T
        return false;
    }
    
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0) {  // I is outside T
        return false;
    }

    return true;
}

