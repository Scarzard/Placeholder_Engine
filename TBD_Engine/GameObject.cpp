#include "GameObject.h"
#include "ModuleSceneIntro.h"

#include "Application.h"

#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Color.h"

#include "Component.h"
#include "Component_Mesh.h"
#include "Component_Transform.h"
#include "Component_Texture.h"
#include "Component_Camera.h"
#include "Component_Animation.h"
#include "Component_Bone.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(std::string name)
{
	this->name = name;
	unactive_name = name + " (not active)";
	this->active = true;
	UUID = App->GetRandomUUID();
	CreateComponent(Component::ComponentType::Transform);
}

GameObject::~GameObject()
{
}

void GameObject::Load(uint obj_num, nlohmann::json & scene_file)
{
	scene_file["Game Objects"][obj_num]["UUID"] = UUID;
	scene_file["Game Objects"][obj_num]["Name"] = name;

	if (parent)
		scene_file["Game Objects"][obj_num]["ParentUUID"] = parent->UUID;
	else
		scene_file["Game Objects"][obj_num]["ParentUUID"] = "NONE";

	scene_file["Game Objects"][obj_num]["Active"] = active;
	scene_file["Game Objects"][obj_num]["Static"] = is_static;

	//save components too
	for (int i = 0; i < components.size(); i++)
		components[i]->Load(obj_num, scene_file);
}

void GameObject::Save(uint obj_num, nlohmann::json &scene)
{
	scene[name.c_str()]["UUID"] = std::to_string(UUID);

	if(parent)
		scene[name.c_str()]["ParentUUID"] = std::to_string(parent->UUID);
	else
		scene[name.c_str()]["ParentUUID"] = "NONE";

	scene[name.c_str()]["Active"] = std::to_string(active);
	scene[name.c_str()]["Static"] = std::to_string(is_static);

	//save components too
	for (int i = 0; i < components.size(); i++)
		components[i]->Save(obj_num, scene);
}

Component* GameObject::CreateComponent(Component::ComponentType type)
{
	
	Component* component = nullptr;

	switch (type)
	{
	case Component::ComponentType::Transform:
		component = new ComponentTransform(this);
		break;
	case Component::ComponentType::Mesh:
		component = new ComponentMesh(this);
		break;
	case Component::ComponentType::Texture:
		component = new ComponentTexture(this);
		break;
	case Component::ComponentType::Camera:
		component = new ComponentCamera(this);
		break;
	case Component::ComponentType::Animation:
		component = new ComponentAnimation(this);
		break;
	case Component::ComponentType::Bone:
		component = new ComponentBone(this);
		break;

	}

	if (component)
		components.push_back(component);

	return component;
}

ComponentMesh* GameObject::GetComponentMesh()
{
	Component* mesh = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Mesh)
		{
			return (ComponentMesh*)*iterator;
		}
	}

	return (ComponentMesh*)mesh;
}

ComponentTransform* GameObject::GetComponentTransform()
{
	Component* transform = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Transform)
		{
			return (ComponentTransform*)*iterator;
		}
	}

	return (ComponentTransform*)transform;
}

ComponentTexture* GameObject::GetComponentTexture()
{
	Component* texture = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Texture)
		{
			return (ComponentTexture*)*iterator;
		}
	}

	return (ComponentTexture*)texture;
}

ComponentCamera * GameObject::GetComponentCamera()
{
	Component* camera = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Camera)
		{
			return (ComponentCamera*)*iterator;
		}
	}

	return (ComponentCamera*)camera;
}

ComponentAnimation * GameObject::GetComponentAnimation()
{
	Component* animation = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Animation)
		{
			return (ComponentAnimation*)*iterator;
		}
	}

	return (ComponentAnimation*)animation;
}

ComponentBone * GameObject::GetComponentBone()
{
	Component* bone = nullptr;
	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		if ((*iterator)->type == Component::ComponentType::Bone)
		{
			return (ComponentBone*)*iterator;
		}
	}

	return (ComponentBone*)bone;
}

void GameObject::Update(float dt)
{
	//update name when GO change state
	std::string str = this->name + " (not active)";
	if(unactive_name.compare(str) != 0)
		unactive_name = name + " (not active)";

	if (this->active)
	{
		if (this->GetComponentTransform()->has_transformed)
		{
			TransformGlobal(this);
			if (this->GetComponentCamera() != nullptr)
			{
				ComponentCamera* camera = this->GetComponentCamera();

				camera->UpdateTransform();
			}
		}

		if (this->GetComponentCamera() != nullptr)
		{
			ComponentCamera* camera = this->GetComponentCamera();
			if(camera->frustum_view && App->gui->game_window->in_editor && App->gui->ins_window->selected_GO == App->camera->obj_camera)
				camera->DrawFrustum();
		}

		if (this->GetComponentBone() != nullptr)
		{
			this->GetComponentBone()->DebugDrawBones();
		}

		if (this->GetComponentAnimation() != nullptr)
		{
			this->GetComponentAnimation()->Update(dt);
		}
		
		ComponentMesh* mesh = this->GetComponentMesh();
		if (mesh != nullptr)
			mesh->UpdateGlobalAABB();
		/*if (mesh->deformable_mesh != nullptr)
			mesh->UpdateMesh();*/
	}

	//Game Object iterative update
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Update(dt);
	}


}

void GameObject::Enable()
{
	if(this->active == false)
		this->active = true;

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Enable();
	}
}

void GameObject::Disable()
{
	if (this->active == true)
		this->active = false;

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Disable();
	}
}

void GameObject::DeleteGO(GameObject* GO, bool original)
{

	//delete its childrens (if it has)
	if (GO->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = GO->children.begin(); it != GO->children.end(); ++it)
		{
			DeleteGO(*it, false);
		}

		GO->children.clear();
	}

	if (GO->parent != nullptr && original == true)
		GO->parent->RemoveChild(GO);

	GO->CleanUp();
	delete GO;
	GO = nullptr;
}

void GameObject::SetChild(GameObject* GO)
{
	if (GO->parent != nullptr)
		GO->parent->RemoveChild(GO);

	GO->parent = this;
	children.push_back(GO);

}

void GameObject::RemoveChild(GameObject* GO)
{
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->id == GO->id)
		{
			children.erase(it);
			break;
		}
	}
}

void GameObject::CleanUp()
{
	if (this->GetComponentMesh() != nullptr)
	{

		GetComponentMesh()->CleanUp();
	}
	
	if (this->GetComponentTexture() != nullptr)
	{

		GetComponentTexture()->CleanUp();
	}

	for (std::vector<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		delete (*iterator);
	}

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		(*it)->CleanUp();
	}

	components.clear();

}

void GameObject::TransformGlobal(GameObject* GO)
{
	ComponentTransform* transform = GO->GetComponentTransform();
	transform->TransformGlobalMat(GO->parent->GetComponentTransform()->GetGlobalTransform());

	for (std::vector<GameObject*>::iterator tmp = GO->children.begin(); tmp != GO->children.end(); ++tmp)
	{
		TransformGlobal(*tmp);
	}
	
}

void GameObject::GetAllChilds(std::vector<GameObject*>& collector)
{
	collector.push_back(this);
	for (uint i = 0; i < children.size(); i++)
		children[i]->GetAllChilds(collector);
}

GameObject* GameObject::GetAnimGO(GameObject* go)
{
	ComponentAnimation* anim = go->GetComponentAnimation();

	if (anim != nullptr)
	{
		return anim->my_GO;
	}
	else
		return GetAnimGO(go->parent);
}