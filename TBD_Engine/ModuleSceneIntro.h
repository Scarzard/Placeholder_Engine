#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

class par_shapes_mesh_s;
class Tree;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Init();
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//Save & Load
	/*void Load(const nlohmann::json  &config);
	void Save(nlohmann::json &config);*/

	void LoadScene(std::string scene_name);
	void LoadGameObjects(nlohmann::json  &scene, GameObject* Root);
	void SaveScene(std::string scene_name);
	void SaveGameObjects(nlohmann::json  &scene, GameObject* Root);

public:

	GameObject* CreateGameObject();
	void DrawRecursively(GameObject* GO);
	GameObject* CollectHits();

	void LoadPrimitiveMesh(const par_shapes_mesh_s* m);

	void CreateCube(float x, float y, float z, float size = 1);
	void CreateSphere(float x, float y, float z, int slices, int stacks);
	void CreateTrefoil(float x, float y, float z, int slices, int stacks, int rad);
	void CreateTorus(float x, float y, float z, int slices, int stacks, int rad);

public:	

	update_status want_to_quit = UPDATE_CONTINUE;
	uint numGO = 0;
	GameObject* root = nullptr;
	Tree* QuadTree = nullptr;

	std::vector<ComponentMesh*> static_meshes;
	std::vector<ComponentMesh*> meshes;

private:
	bool skip_tree = true;
};
