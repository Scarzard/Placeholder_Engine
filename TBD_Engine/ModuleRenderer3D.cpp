#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SpacePartition.h"
#include "ModuleSceneIntro.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ModuleEngineUI.h"
#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment(lib, "glew/libx86/glew32.lib")

#include "mmgr/mmgr.h"

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	App->LogInConsole("Creating 3D Renderer context");

	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if(context == NULL)
	{
		App->LogInConsole("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			App->LogInConsole("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		// Initialize glew
		GLenum error = glewInit();
		App->LogInConsole("Using Glew %s", glewGetString(GLEW_VERSION));

		if (error != GL_NO_ERROR)
		{
			App->LogInConsole("Error initializing glew! %s\n"/*, glewGetErrorString(error)*/);
			ret = false;
		}


		//To detect our current hardware and driver capabilities
		App->LogInConsole("Vendor: %s", glGetString(GL_VENDOR));
		App->LogInConsole("Renderer: %s", glGetString(GL_RENDERER));
		App->LogInConsole("OpenGL version supported %s", glGetString(GL_VERSION));
		App->LogInConsole("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LogInConsole("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LogInConsole("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->LogInConsole("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		
		lights[0].Active(true);
	
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);


	}
	OnResize(App->window->width, App->window->height);

	return ret;
}

bool ModuleRenderer3D::Start()
{

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	bool* update_camera = App->camera->GetProjectionUpdateFlag();
	if (*update_camera)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glLoadMatrixf((GLfloat*)App->camera->GetOpenGLProjection());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		*update_camera = false;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetOpenGLView());

	// light 0 on cam pos
//lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	//Draw UI
	App->gui->Draw();

	//LAST THING TO DO IN POSTUPDATE
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	App->LogInConsole("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	ComponentCamera* tmp = App->camera->GetActiveCamera();
	if (tmp != nullptr)
		tmp->has_transformed = true;
	else
		SDL_assert(false);
}

void ModuleRenderer3D::NewVertexBuffer(float3 * vertex, uint &size, uint &id_vertex)
{
	glGenBuffers(1, (GLuint*) &(id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * size, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::NewIndexBuffer(uint *index, uint &size, uint &id_index)
{
	glGenBuffers(1, (GLuint*) &(id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * size, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::NewTexBuffer(float * tex_coords, uint & num_tex_coords, uint & id_tex_coords)
{
	glGenBuffers(1, (GLuint*) &(id_tex_coords)); 
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coords); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords*2, tex_coords, GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void ModuleRenderer3D::Draw(GameObject* m) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (m->GetComponentTexture() != nullptr)
	{
		if (m->GetComponentTexture()->Checers_texture == true)
			glBindTexture(GL_TEXTURE_2D, App->tex_loader->CheckersTexture.id);
		else
		{
			if(m->GetComponentTexture()->res_texture)
				glBindTexture(GL_TEXTURE_2D, m->GetComponentTexture()->res_texture->texture);
		}

	}

	if (m->GetComponentMesh() != nullptr)
	{
		ComponentMesh* mesh = m->GetComponentMesh();

		if (m->GetComponentMesh()->res_mesh != nullptr)
		{
			if (mesh->deformable_mesh)
			{
				glBindBuffer(GL_ARRAY_BUFFER, mesh->deformable_mesh->id_vertex);

				if(mesh->my_GO->parent->GetComponentAnimation()->draw_bones)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else 
				glBindBuffer(GL_ARRAY_BUFFER, mesh->res_mesh->id_vertex);

			glVertexPointer(3, GL_FLOAT, 0, NULL);

			if (mesh->res_mesh->num_tex_coords > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, mesh->res_mesh->id_tex_coords);
				glTexCoordPointer(2, GL_FLOAT, 0, NULL);
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh->res_mesh->id_index);
			glDrawElements(GL_TRIANGLES, mesh->res_mesh->num_index, GL_UNSIGNED_INT, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisableClientState(GL_VERTEX_ARRAY);

			if (mesh->draw_normals)
			{
				glBegin(GL_LINES);
				glColor4f(Red.r, Red.g, Red.b, Red.a);

				for (int j = 0; j < mesh->res_mesh->num_normals; ++j)
				{
					glVertex3f(mesh->res_mesh->face_center[j].x, mesh->res_mesh->face_center[j].y, mesh->res_mesh->face_center[j].z);
					glVertex3f(mesh->res_mesh->face_center[j].x + mesh->res_mesh->face_normal[j].x, mesh->res_mesh->face_center[j].y + mesh->res_mesh->face_normal[j].y, mesh->res_mesh->face_center[j].z + mesh->res_mesh->face_normal[j].z);
				}
				glColor4f(White.r, White.g, White.b, White.a);

				glEnd();
			}

			if (App->gui->conf_window->draw_aabb)
			{
				mesh->DrawAABB();
			}
		}

		
	}

}

void ModuleRenderer3D::UpdateBuffer(ComponentMesh* mesh)
{
	if (mesh->deformable_mesh != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->deformable_mesh->id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * mesh->deformable_mesh->num_vertex, mesh->deformable_mesh->vertex, GL_STATIC_DRAW);
	}
}
