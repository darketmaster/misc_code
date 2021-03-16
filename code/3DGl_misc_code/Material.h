#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <stdio.h>
#include <GL/gl.h>

#define MAT3DHEADER "MAT3D" 

#define MAT3D_MAX_NAME             32

class MATERIAL
{
public:

	char  Name[MAT3D_MAX_NAME];
	float Ambient[4];
	float Diffuse[4];
	float Specular[4];
	float Emissive[4];
	float Shininess;
	float Transparency;
	char  DiffuseTexture[MAT3D_MAX_NAME];
	char  AlphaTexture[MAT3D_MAX_NAME];
	GLuint texture;
	GLuint Atexture;
 
  	MATERIAL();
	~MATERIAL();
	void clear();
	void destroy();
	
	int  loadFromMs3dAsciiSegment( FILE *f );
    int  save_txt_chunk(FILE *f);
    int  load_txt_chunk(FILE *f);
    int  save_bin_chunk(FILE *f);
    int  load_bin_chunk(FILE *f);
    int  save_txt(char *filename);
    int  load_txt(char *filename);
    int  save_bin(char *filename);
    int  load_bin(char *filename);	
	
	void activate( void );
	void reloadTexture( void );
};


#endif          /* ifndef _MATERIAL_H_ */
	
