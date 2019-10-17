#include "MeshLoader.h"
#include "Application.h"


#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"



MeshLoader::MeshLoader(bool start_enabled) : Module(start_enabled)
{
}

MeshLoader::~MeshLoader()
{}



bool MeshLoader::Init()
{

	// Stream log messages to Debug window 
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool MeshLoader::Start()
{

	return true;
}

update_status MeshLoader::Update(float dt)
{
	//Draw meshes
	for (int i = 0; i < LoadedMeshes.size(); ++i)
		App->renderer3D->Draw(LoadedMeshes[i]);

	return UPDATE_CONTINUE;
}

bool MeshLoader::CleanUp()
{
	// detach log stream 
	aiDetachAllLogStreams();



	return true;
}

void MeshLoader::LoadFile(const char* full_path)
{
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->HasMeshes()) //Load sucesful
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			MeshInfo* m = new MeshInfo;

			aiMesh* new_mesh = scene->mMeshes[i];

			m->num_vertex = new_mesh->mNumVertices;
			m->vertex = new float3[m->num_vertex];

			for (uint i = 0; i < new_mesh->mNumVertices; ++i)
			{
				m->vertex[i].x = new_mesh->mVertices[i].x;
				m->vertex[i].y = new_mesh->mVertices[i].y;
				m->vertex[i].z = new_mesh->mVertices[i].z;
			}

			// copy faces
			if (new_mesh->HasFaces())
			{
				m->num_index = new_mesh->mNumFaces * 3;
				m->index = new uint[m->num_index]; // assume each face is a triangle
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
						App->LogInConsole("WARNING, geometry face with != 3 indices!");
					else
						memcpy(&m->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}

			if (new_mesh->HasTextureCoords(0))
			{
				m->num_tex_coords = m->num_vertex;
				m->tex_coords = new float[m->num_tex_coords * 2];

				for (int i = 0; i < m->num_tex_coords; ++i)
				{
					m->tex_coords[i * 2] = new_mesh->mTextureCoords[0][i].x;
					m->tex_coords[(i * 2) + 1] = new_mesh->mTextureCoords[0][i].y;
				}
			}

			//Generate the buffers 
			App->renderer3D->NewVertexBuffer(m->vertex, m->num_vertex, m->id_vertex);
			App->renderer3D->NewIndexBuffer(m->index, m->num_index, m->id_index);
			//Generate the buffer for the tex_coordinates
			App->renderer3D->NewTexBuffer(m->tex_coords, m->num_tex_coords, m->id_tex_coords);


			//Add Loaded mesh to the array f meshes
			LoadedMeshes.push_back(m);
		}
		aiReleaseImport(scene);


	}
	else
		App->LogInConsole("Error loading scene %s", full_path);
}