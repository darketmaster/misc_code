#ifndef _MESH_H_
#define _MESH_H_

#include <stdio.h>
#include <GL/gl.h>

#include "3DMATRIX.H"

#define MESH3DHEADER "MESH3D" 

#define MESH_MAX_NAME             32

class _TRI
{
public:
	int		v[3];	// indices vertices[]
	int		n[3];	// indices normals[], normal per vertex
};

class Mesh 
{
	public:
	    
        char Name[MESH_MAX_NAME];
        int material_index;
       
		int	num_vertices;
		V3D_f	*vertices;

		int	num_normals;
		CVector3	*normals;
		
		int	num_triangles;
		_TRI	*triangles;
  
    	Mesh();
		~Mesh();
		void clear();
		void destroy();
		
        int  save_txt_chunk(FILE *f);
        int  load_txt_chunk(FILE *f);
        int  save_bin_chunk(FILE *f);
        int  load_bin_chunk(FILE *f);
        int  save_txt(char *filename);
        int  load_txt(char *filename);
        int  save_bin(char *filename);
        int  load_bin(char *filename);
		int  loadFromMs3dAsciiSegment( FILE *f );
		void render( void );
};


#endif          /* ifndef _MESH_H_ */
	
