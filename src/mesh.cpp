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

bool Mesh::intersectTriangle(Point3D v0, Point3D v1, Point3D v2, 
			Point3D origin, Vector3D ray, Point3D &intersection, Vector3D &normal)
{
    // This always returns true, lets try again 
    /*Vector3D u = b-a;
    Vector3D v = c-a; 	

    Vector3D w = origin - a;

    normal = u.cross(v);

    double s = (u.dot(v)) * (w.dot(u)) - (v.dot(v)) * (w.dot(v));
    s = s / ((u.dot(v)) * (u.dot(v)) - (u.dot(u)) * (v.dot(v)));

    double t = (u.dot(v)) * (w.dot(u)) - (u.dot(u)) * (w.dot(v));
    t = t / ((u.dot(v)) * (u.dot(v)) - (u.dot(u)) * (v.dot(v)));

    
    if(s + t > 1)
    {
        std::cerr << "No Intersection" << endl;	
        return false;
    }

    intersection = a + s*u + t*v;	
    return true;*/

    Vector3D    I; // Intersection Point
    Vector3D    u, v;              // triangle vectors
    Vector3D    dir, w0, w;           // ray vectors
    float     r, a, b;              // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = v1-v0;
    v = v2-v0;
    normal = u.cross(v);              // cross product
    if (normal[0] == 0 && normal[1] == 0 && normal[2] == 0) {             // triangle is degenerate
        //std::cerr << "Option F" << std::endl; 
        return false;                  // do not deal with this case
    }

    // dir = R.P1 - R.P0;              // ray direction vector -> This is our Ray
    w0 = Vector3D(origin[0] - v0[0], origin[1] - v0[1], origin[2] - v0[2]);
    a = normal.dot(w0) * -1;
    b = normal.dot(ray);
    if (fabs(b) < 0.00001) {     // ray is  parallel to triangle plane
        if (a == 0) {                // ray lies in triangle plane
            //std::cerr << "Option D" << std::endl;
            return false;
        }

        else { 
            //std::cerr << "Option E" << std::endl;
            return false;              // ray disjoint from plane
        }
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0){                    // ray goes away from triangle
        //std::cerr << "Option G" << std::endl;
        return false;                   // => no intersect
    }
    // for a segment, also test if (r > 1.0) => no intersect

    Vector3D rRay = Vector3D(r*ray[0], r*ray[1], r*ray[2]);
    I = Vector3D(origin[0] + rRay[0], origin[1] + rRay[1], origin[2] + rRay[2]);            // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;

    uu = u.dot(u);
    uv = u.dot(v);
    vv = v.dot(v);
    w = Vector3D(I[0] - v0[0], I[1] - v0[1], I[2] - v0[2]);
    wu = w.dot(u);
    wv = w.dot(v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0){         // I is outside T
        //std::cerr << "Option C" << std::endl;
        return false;
    }
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0) {  // I is outside T
        //std::cerr << "Option B" << std::endl; 
        return false;
    }

    //std::cerr << "Option A" << std::endl;
    return true;                       // I is in T
}

