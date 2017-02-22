#include <assert.h>
#include <math.h>
#include "a4.hpp"
#include "image.hpp"

const double pi = 3.1415926535897;

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
               )
{
  // Fill in raytracing code here.

 // std::cerr << "Stub: a4_render(" << root << ",\n     "
  //          << filename << ", " << width << ", " << height << ",\n     "
   //         << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
    //        << ambient << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "});" << std::endl;
  
	// Setup device to world coordinate matrix

	Matrix4x4 t4, r3, s2, t1;

	{
	// Translate (T4)
	Vector4D a = Vector4D(1, 0, 0, eye[0]);
	Vector4D b = Vector4D(0, 1, 0, eye[1]);
	Vector4D c = Vector4D(0, 0, 1, eye[2]);
	Vector4D d = Vector4D(0, 0, 0, 1);

	t4 = Matrix4x4(a, b, c, d);
	}

	{
	Vector3D w = Vector3D(view[0] / view.length(), view[1] / view.length(), view[2]/view.length());
	
	Vector3D u = up.cross(w);
	u[0] = u[0] / u.length();
	u[1] = u[1] / u.length();
	u[2] = u[2] / u.length();

	Vector3D v = w.cross(u);

	// Rotate (R3)
	Vector4D a = Vector4D(u[0], v[0], w[0], 0);
	Vector4D b = Vector4D(u[1], v[1], w[1], 0);
	Vector4D c = Vector4D(u[2], v[2], w[2], 0);
	Vector4D d = Vector4D(0, 0, 0, 1);

	r3 = Matrix4x4(a, b, c, d);
	}

	{
	//Scale (S2)

	double fovRad = fov/180 * pi;

	double h = 2 * view.length() * tan(fovRad / 2);
	double w = width / height * h;

	Vector4D a = Vector4D(-1 * h / height, 0, 0, 0);
	Vector4D b = Vector4D(0, -1 * w / width, 0, 0);
	Vector4D c = Vector4D(0, 0, 1, 0);
	Vector4D d = Vector4D(0, 0, 0, 1);

	s2 = Matrix4x4(a, b, c, d);
	}

	{
	//Translate (T1)
	Vector4D a = Vector4D(1, 0, 0, -1 * width / 2);
	Vector4D b = Vector4D(0, 1, 0, -1 * height / 2);
	Vector4D c = Vector4D(0, 0, 1, view.length());
	Vector4D d = Vector4D(0, 0, 0, 1);

	t1 = Matrix4x4(a, b, c, d);
	}

	Matrix4x4 transform = t4*r3*s2*t1;
  Image img(width, height, 3);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      
			Point3D pixelPoint = Point3D(x, y, 0);
			Point3D worldPoint = transform * pixelPoint; 

			Vector3D ray = worldPoint - eye;
			ray.normalize();

			Colour c = getColourAtPoint(eye, ray, lights, eye, ambient, root);

			img(x, y, 0) = c.R();
			img(x, y, 1) = c.G();
			img(x, y, 2) = c.B();
		
			// Red: increasing from top to bottom
      //img(x, y, 0) = (double)y / height;
      // Green: increasing from left to right
      //img(x, y, 1) = (double)x / width;
      // Blue: in lower-left and upper-right corners
      //img(x, y, 2) = ((y < height/2 && x < height/2)
      //               || (y >= height/2 && x >= height/2)) ? 1.0 : 0.0;
    }
  }	

  img.savePng(filename);  
}

Colour getColourAtPoint(Point3D origin, Vector3D ray, std::list<Light*> lights, Point3D eye, Colour ambient, SceneNode* root){
	
		Colour c_kd = Colour(0,0,0);
		Colour c_ks = Colour(0,0,0);
		Colour c_ke = Colour(0,0,0);

		Vector3D normal;
		double c_t;
		double c_shininess;

		double a = 1;

		if(hitAnything(origin, ray, c_t, normal, c_kd, c_ks, c_ke, c_shininess, root)){
			
			//std::cerr << "we hit " << c_ke << ".\n";	

			Colour retColour = a * ambient;					

			//std::cerr << "we pass";

			for(std::list<Light*>::iterator it=lights.begin(); it != lights.end(); ++it)
			{
				// KD
				Light* light = *it;

				Point3D pointOnSurface = origin + c_t*ray;
				Vector3D dirToLight = light->position - pointOnSurface;			
					
				dirToLight.normalize();
				normal.normalize();	

				//std::cerr << "R: " << c_kd.R() << "\n";				

				//retColour = retColour + (c_kd * dirToLight.dot(normal) * light->colour);

				//std::cerr << "a " << normal << "\n";				
				//std::cerr << "b " << dirToLight << "\n";				
				
				//std::cerr << "numba " << dirToLight.dot(normal) << "\n";				
				//std::cerr << "retColour: " << retColour.R() << "\n";				

				pointOnSurface = pointOnSurface + 1*dirToLight;
				//dirToLight = Vector3D(0, 0, 0);
				//redo it...
				//dirToLight = light->position - pointOnSurface;			

				//std::cerr << "pointa " << pointOnSurface << "\n";				
				//std::cerr << "dir " << dirToLight << "\n";				

				// Check the shadow ray
				if(!hitAnything(pointOnSurface, dirToLight, root)){

				retColour = retColour + (c_kd * dirToLight.dot(normal) * light->colour);

				//if(dirToLight.dot(normal) > 0){
					// KS
					Vector3D reflectedDirection = 2 * (dirToLight.dot(normal)) * (normal - dirToLight); 
					Vector3D toViewer = eye - pointOnSurface;

					//std::cerr << "eye " << eye << "\n";				
			
					reflectedDirection.normalize();
					toViewer.normalize();
		
					retColour = retColour + (c_ks * pow(reflectedDirection.dot(toViewer), c_shininess) 
						* light->colour);

					//std::cerr << "c " << pow(reflectedDirection.dot(toViewer), c_shininess) << "\n";				
					//std::cerr << "UPretColour: " << retColour.R() << "\n";				
				//}
				} // end hit check	

			}

			return retColour;
		}

	else return Colour(0.1, 0.1, ray[1]);
}

bool hitAnything (Point3D rayOrigin, Vector3D ray, SceneNode* root){

		Colour c_kd = Colour(0,0,0);
		Colour c_ks = Colour(0,0,0);
		Colour c_ke = Colour(0,0,0);

		Vector3D normal;
		double c_t;
		double c_shininess;

		return hitAnything (rayOrigin, ray, c_t, normal, c_kd, c_ks, c_ke, c_shininess, root);
}

bool hitAnything (Point3D rayOrigin, Vector3D ray, double &t, Vector3D &normal, 
	Colour &kd, Colour &ks, Colour &ke, double &shininess, SceneNode* root)
{

	// Cycle through each object and check for a collision with this ray
	
	t = 100000;
	bool hitAnything = false;

	// For now, deal with nonheir objects only
	std::list<SceneNode*> children = root->getChildren();
	for(std::list<SceneNode*>::iterator it = children.begin(); 
			it != children.end(); ++it)
	{
		GeometryNode* node = (GeometryNode*) *it;

		Colour c_kd = Colour(0,0,0);
		Colour c_ks = Colour(0,0,0);
		Colour c_ke = Colour(0,0,0);
		
		double c_t = 1000000;
		double c_shininess = 0;

		bool hit = node->checkCollision(rayOrigin, ray, c_t, normal, 
			c_kd, c_ks, c_ke, c_shininess);

		if(hit && c_t < t)
		{
			hitAnything = true;			

		//	std::cerr<< "Updating";

			t = c_t;			
			kd = c_kd;
			ks = c_ks;
			ke = c_ke;
			
			shininess = c_shininess;
			//std::cerr<<"updated\n";
		}
	}

	return hitAnything;
}

