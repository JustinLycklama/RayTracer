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

    std::cerr << "Render ..." << std::endl;
    std::cerr << std::endl;

    // Print all of the lights in our scene
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
  
        // 3 elements per pixel. R, G, B.
        Image img(width, height, 3);

    std::cerr << "Begin creating light rays" << std::endl;

    // Perform entire image creation. Transform a pixel point into a world point, create a vector from that pixel to our eye, 
    // and find what color that ray of light will be.
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


    std::cerr << "Create Image: " << filename << std::endl;
    img.savePng(filename);  
}

Colour getColourAtPoint(Point3D origin, Vector3D ray, std::list<Light*> lights, Point3D eye, Colour ambient, SceneNode* root)
{
    Colour c_kd = Colour(0,0,0);
    Colour c_ks = Colour(0,0,0);
    Colour c_ke = Colour(0,0,0);

    Vector3D normal;
    double c_t;
    double c_shininess;

    if(hitAnything(origin, ray, c_t, normal, c_kd, c_ks, c_ke, c_shininess, root)) {
			
	    Colour retColour = 1 * ambient;					

	    for(std::list<Light*>::iterator it=lights.begin(); it != lights.end(); ++it)
	    {   
            Light* light = *it;

            Point3D pointOnSurface = origin + c_t*ray;
            Vector3D dirToLight = light->position - pointOnSurface;	

            dirToLight.normalize();
            normal.normalize();	

            // Remove interference. If we trace directly from our point back to the light, 
            // theres a chance we'd collide with the same thing again. 

            // A new point, one unit vector away from intersection point, towards the light
            pointOnSurface = pointOnSurface + dirToLight;

            // Check the shadow ray
            // If we don't hit anything, add diffuse and specular light
            if(!hitAnything(pointOnSurface, dirToLight, root))  
            {
                float specular = 0.0;

                // Lambert's cosine law
                float lambertian = normal.dot(dirToLight);

                if (lambertian < 0) {
                    lambertian = 0;
                }
                else if (lambertian > 0) {
                    Vector3D dirToViewer = -dirToLight;
                    Vector3D reflectedDirection = reflectRayAlongNormal(ray, normal);

                    float specAngle = reflectedDirection.dot(dirToViewer);
                    specular = pow(specAngle, c_shininess);
                }

                // Add diffuse and specular light
                retColour = retColour + 
                            c_kd * lambertian * light->colour +
                            c_ke * specular * light->colour;
            }
        }

        return retColour;
    }
    else {
        // Return the background color
        return Colour(0.1, 0.1, ray[1]);
    }
}

// Reflection is r = d−2(d⋅n)n
Vector3D reflectRayAlongNormal(Vector3D ray, Vector3D normal) 
{
    return ray - 2 * (ray.dot(normal)) * normal;
}

bool hitAnything (Point3D rayOrigin, Vector3D ray, SceneNode* root)
{
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

                    t = c_t;			
                    kd = c_kd;
                    ks = c_ks;
                    ke = c_ke;
                    
                    shininess = c_shininess;
            }
    }

    return hitAnything;
}

