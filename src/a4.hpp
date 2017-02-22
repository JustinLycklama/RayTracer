#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               );

Colour getColourAtPoint(Point3D origin, Vector3D ray, std::list<Light*> lights, Point3D eye, Colour ambient, SceneNode* root);

bool hitAnything (Point3D origin, Vector3D ray, SceneNode* root); 

bool hitAnything (Point3D origin, Vector3D ray, double &t, Vector3D &normal, 
	Colour &kd, Colour &ks, Colour &ke, double &shininess, SceneNode* root);

/*bool hitNode (Vector3D ray, double &t, Vector3D normal, Point3D uv, 
	Colour &kd, Colour &ks, Colour &ke, SceneNode* node);
*/


#endif
