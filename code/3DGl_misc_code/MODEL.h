#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdio.h>
#include <GL/gl.h>

#include "3DMATRIX.H"
#include "bones.h"
#include "Material.h"
#include "MESH.H" 

#define MODEL3DHEADER "MODEL3D" 

class MODEL3D
{
	public:

		int		num_meshes;
		Mesh	*meshes;

		int		num_materials;
		MATERIAL *materials;

        BONES3D *bones;
		
		MODEL3D();
		~MODEL3D();
		void clear();
		void destroy();
		int loadFromMs3dAsciiFile( const char *filename );
};



#endif          /* ifndef _MODEL_H_ */
	
