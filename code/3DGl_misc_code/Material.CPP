#include <stdlib.h>
#include "MATERIAL.H" 
#include "filelog.h" 
#include <stdlib.h>
#include <string.h>
#include "Texture.h"
#include "Material.h"


#define get_new_line(file) if(!fgets (szLine, 256, (file))){ return 0; }



MATERIAL::MATERIAL()
{
    clear();
}
    
MATERIAL::~MATERIAL()
{
    destroy();
}
    
void MATERIAL::clear()
{
    memset(Name, 0, MAT3D_MAX_NAME * sizeof(char));
    memset(DiffuseTexture, 0, MAT3D_MAX_NAME * sizeof(char));
    memset(AlphaTexture, 0, MAT3D_MAX_NAME * sizeof(char));
    
    memset(Ambient, 0, 4 * sizeof(float));
    memset(Diffuse, 0, 4 * sizeof(float));
    memset(Specular, 0, 4 * sizeof(float));
    memset(Emissive, 0, 4 * sizeof(float));
    
	Shininess = 0.0;
	Transparency = 0.0;
	
	texture = 0;
	Atexture = 0;

}
    
void MATERIAL::destroy()
{
}
    
	
int MATERIAL::load_txt_chunk(FILE *f)
{
    char szLine[256];

    // name
    get_new_line(f);
    if (sscanf (szLine, "\"%[^\"]\"", Name) != 1)
		return 0;

	// ambient
    get_new_line(f);
	if (sscanf (szLine, "%f %f %f %f", &Ambient[0], &Ambient[1], &Ambient[2], &Ambient[3]) != 4)
		return 0;

    // diffuse
    get_new_line(f);
    if (sscanf (szLine, "%f %f %f %f", &Diffuse[0], &Diffuse[1], &Diffuse[2], &Diffuse[3]) != 4)
		return 0;

    // specular
    get_new_line(f);
    if (sscanf (szLine, "%f %f %f %f", &Specular[0], &Specular[1], &Specular[2], &Specular[3]) != 4)
		return 0;

    // emissive
    get_new_line(f);
    if (sscanf (szLine, "%f %f %f %f", &Emissive[0], &Emissive[1], &Emissive[2], &Emissive[3]) != 4)
		return 0;

    // shininess
    get_new_line(f);
    if (sscanf (szLine, "%f", &Shininess) != 1)
		return 0;

    // transparency
    get_new_line(f);
    if (sscanf (szLine, "%f", &Transparency) != 1)
		return 0;

    // diffuse texture
    get_new_line(f);
	strcpy(DiffuseTexture, "");
    sscanf (szLine, "\"%[^\"]\"", DiffuseTexture);

    // alpha texture
    get_new_line(f);
	strcpy(AlphaTexture, "");
    sscanf (szLine, "\"%[^\"]\"", AlphaTexture);
    
    add_in_log("RELOADING TEXTURES..\n");
	reloadTexture();

	return true;
}
    
int  MATERIAL::save_txt_chunk(FILE *f)
{
    if(!f) return 0;
    
    fprintf(f, "\"%s\"\n", Name );
    
    fprintf(f, "%f %f %f %f", Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
    fprintf(f, "%f %f %f %f", Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
    fprintf(f, "%f %f %f %f", Specular[0], Specular[1], Specular[2], Specular[3]);
    fprintf(f, "%f %f %f %f", Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
    fprintf(f, "%f", Shininess);
    fprintf(f, "%f", Transparency);
    
    fprintf(f, "\"%s\"\n", DiffuseTexture );
    fprintf(f, "\"%s\"\n", AlphaTexture );

}
    
int  MATERIAL::loadFromMs3dAsciiSegment( FILE *f )
{
    return load_txt_chunk(f);
}
    
int  MATERIAL::save_bin_chunk(FILE *f)
{
    if(!f) return 0; 
    
    fwrite(Name, sizeof(char), MAT3D_MAX_NAME, f);
    
    fwrite(Ambient, sizeof(float), 4, f);
    fwrite(Diffuse, sizeof(float), 4, f);
    fwrite(Specular, sizeof(float), 4, f);
    fwrite(Emissive, sizeof(float), 4, f);
    
    fwrite(&Shininess, sizeof(float), 1, f);
    fwrite(&Transparency, sizeof(float), 1, f);
	
    fwrite(DiffuseTexture, sizeof(char), MAT3D_MAX_NAME, f);
    fwrite(AlphaTexture, sizeof(char), MAT3D_MAX_NAME, f);    

    return 1;
}
    
int  MATERIAL::load_bin_chunk(FILE *f)
{
    if(!f) return 0; 
    
    fread(Name, sizeof(char), MAT3D_MAX_NAME, f);
    
    fread(Ambient, sizeof(float), 4, f);
    fread(Diffuse, sizeof(float), 4, f);
    fread(Specular, sizeof(float), 4, f);
    fread(Emissive, sizeof(float), 4, f);
    
    fread(&Shininess, sizeof(float), 1, f);
    fread(&Transparency, sizeof(float), 1, f);
	
    fread(DiffuseTexture, sizeof(char), MAT3D_MAX_NAME, f);
    fread(AlphaTexture, sizeof(char), MAT3D_MAX_NAME, f);    

    return 1;
}
    
int  MATERIAL::save_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wt");
    if(f)
    {
        signature_save_file_t(f,MAT3DHEADER);
        save_txt_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1; 
}
    
int  MATERIAL::load_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rt");
    if(f)
    {
        if(!signature_load_file_t(f,MAT3DHEADER))
        {
            fclose(f);
            return 0;
        }    
        load_txt_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;
}
    
int  MATERIAL::save_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wb");
    if(f)
    {
        signature_save_file_b(f, strlen(MAT3DHEADER), MAT3DHEADER);
        save_bin_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;  
}
    
int  MATERIAL::load_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rb");
    if(f)
    {
        if(!signature_load_file_b(f, strlen(MAT3DHEADER), MAT3DHEADER))
        {
            fclose(f);
            return 0;
        }    
        load_bin_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;
}
    	
	
void MATERIAL::activate( void )
{
    glMaterialfv( GL_FRONT, GL_AMBIENT, Ambient );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Diffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, Specular );
	glMaterialfv( GL_FRONT, GL_EMISSION, Emissive );
	glMaterialf( GL_FRONT, GL_SHININESS, Shininess );

	if ( texture > 0 )
	{
		glBindTexture( GL_TEXTURE_2D, texture );
		glEnable( GL_TEXTURE_2D );
	}
	else
		glDisable( GL_TEXTURE_2D );
}
    
void MATERIAL::reloadTexture( void )
{
    if(texture)
       glDeleteTextures(1, &texture);
    texture = 0;
		
	add_in_log("LOADING TEXTURES\n");	
	tMaterial m_pMaterials;

	if(( strlen(DiffuseTexture) > 0 )&& (fexists(DiffuseTexture)))
	{
		LoadTexture(DiffuseTexture, &m_pMaterials, true);
		texture = m_pMaterials.m_id;
		add_in_log("TEXTURE: %s : %d\n", DiffuseTexture, m_pMaterials.m_id);
    }		
	else
		texture = 0;
}
    
