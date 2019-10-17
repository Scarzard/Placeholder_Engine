#include "Shapes.h"
#include "Application.h"
#include "Module.h"

#include "par/par_shapes.h"
#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */


Shapes::Shapes() {}

Shapes::~Shapes() {}

void Shapes::CreateBuffer()
{
	if (obj != nullptr)
	{
		//vertex
		glGenBuffers(1, (GLuint*) &(id_vertex));
		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj->npoints * 3, obj->points, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//indices
		glGenBuffers(1, (GLuint*) &(id_indices));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * obj->ntriangles * 3, obj->triangles, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//texture
		glGenBuffers(1, (GLuint*)&(id_texture)); // 
		glBindBuffer(GL_ARRAY_BUFFER, id_texture);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj->npoints*6, obj->tcoords, GL_STATIC_DRAW); 
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
	}
}

void Shapes::RenderShape()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	if (type == SPHERE)
	{
		//texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glBindBuffer(GL_ARRAY_BUFFER, id_texture);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	

	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glDrawElements(GL_TRIANGLES, obj->ntriangles * 3, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (type == SPHERE)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
}

// CREATE SHAPES FUNCTIONS //

void Shapes::CreateSphere(float x, float y, float z, int slices, int stacks)
{
	//There has to be at least 3 slices and at least 3 stacks, otherwise it won't create the sphere :(

	obj = par_shapes_create_parametric_sphere(slices, stacks);

	type = SPHERE;

	position.x = x;
	position.y = y;
	position.z = z;

	par_shapes_translate(obj, position.x, position.y, position.z);

	Texture = App->tex_loader->id_checkersTexture;

	CreateBuffer();
}

void Shapes::CreateTrefoil(float x, float y, float z, int slices, int stacks, int rad)
{
	//There has to be at least 3 slices and at least 3 stacks, otherwise it won't create the trefoil:(. And radius little than 3 better

	obj = par_shapes_create_trefoil_knot(slices, stacks, rad);
	position.x = x;
	position.y = y;
	position.z = z;

	par_shapes_translate(obj, position.x, position.y, position.z);

	CreateBuffer();
}

void Shapes::CreateIcosahedron(float x, float y, float z)
{
	obj = par_shapes_create_icosahedron();
	position.x = x;
	position.y = y;
	position.z = z;

	par_shapes_translate(obj, position.x, position.y, position.z);

	CreateBuffer();
}

void Shapes::CreateCube(float x, float y, float z, float size)
{
	obj = par_shapes_create_cube();
	position.x = x;
	position.y = y;
	position.z = z;

	par_shapes_scale(obj, size, size, size);
	par_shapes_translate(obj, position.x, position.y, position.z);

	CreateBuffer();
}