#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  //std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
  // Fill me in
}

void SceneNode::scale(const Vector3D& amount)
{
  //std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
  // Fill me in
}

void SceneNode::translate(const Vector3D& amount)
{
  //std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
  // Fill me in
}

bool SceneNode::is_joint() const
{
  return false;
}

std::list<SceneNode*> SceneNode::getChildren(){
	return m_children;
} 

bool SceneNode::checkCollision(Point3D rayOrigin, Vector3D ray, 
	double &t, Vector3D &normal, 
	Colour &kd, Colour &ks, Colour &ke, double &shininess){
	
	return false;
}

// Joint Node

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

bool JointNode::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal,
		Colour &kd, Colour &ks, Colour &ke, double &shininess)
{
	return false;
}

// GeometryNode

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::checkCollision(Point3D rayOrigin, Vector3D ray, 
		double &t, Vector3D &normal, 
		Colour &kd, Colour &ks, Colour &ke, double &shininess)
{
	//std::cerr<<"test: " << m_name << ".\n";
	
	bool hit = false;;
	
	NonhierSphere* sphere =  dynamic_cast<NonhierSphere*>(m_primitive);
	if(sphere != NULL){
		hit = sphere->checkCollision(rayOrigin, ray, t, normal);
	}
	else{
		Mesh* mesh =  dynamic_cast<Mesh*>(m_primitive);
		
		if(mesh != NULL){
			//hit = mesh->checkCollision(rayOrigin, ray, t, normal);
		}
	}
	
	if(!hit)
		return false;

	PhongMaterial* mat = (PhongMaterial*) m_material;

	//std::cerr<<"we SET";

	kd = Colour(mat->m_kd);
	
	//std::cerr<<"we set 2";
	ks = Colour(mat->m_ks);
	ke = Colour(0, 0, 0); // dunno

	shininess = mat->m_shininess;

	//std::cerr<<"we finish set\n";

	return true;
}


 
