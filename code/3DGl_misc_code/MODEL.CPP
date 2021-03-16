#include <stdlib.h>
#include "MODEL.H" 
#include "filelog.h" 
#include <stdlib.h>
#include <string.h>
#include "bones.H" 
#include "material.H" 

#define get_new_line(file) if(!fgets (szLine, 256, (file))){ return 0; }

/*
 * MODEL3D 
 */

MODEL3D::MODEL3D()
{
    clear();
}



MODEL3D::~MODEL3D()
{
    destroy();
}


void MODEL3D::clear()
{
    num_meshes = 0;
	meshes = NULL;
	num_materials = 0;
	materials = NULL;
	bones = NULL;
} 

void MODEL3D::destroy()
{
    if(meshes != NULL)
	{
		delete[] meshes;
		meshes = NULL;
	}
	if(materials != NULL)
	{
		delete[] materials;
		materials = NULL;
	}
	if(bones)
    {  
        delete bones;
        bones = 0;
    }    
    num_meshes = 0;
	num_materials = 0;
}   


int MODEL3D::loadFromMs3dAsciiFile( const char *filename )
{
	//float	StartTime;
    int	bError = 0;
    char	szLine[256];
    int		nFlags, nIndex, i;
    int nFrame, StartTime, CurrentTime;
    
    add_in_log("LOADING MODEL: %s\n", filename);

	FILE *file = fopen (filename, "rt");
	if (!file)
		return false;

	CurrentTime = 0;

    while (fgets (szLine, 256, file) != NULL  && !bError)
    {
        if (!strncmp (szLine, "//", 2))
            continue;

        if (sscanf (szLine, "Frames: %d", &nFrame) == 1)
            continue;
        
        if (sscanf (szLine, "Frame: %d", &StartTime) == 1)
            continue;

        if (sscanf (szLine, "Meshes: %d", &num_meshes) == 1)
        {
			meshes = new Mesh[num_meshes];

            for (i = 0; i < num_meshes && !bError; i++)
            {

                get_new_line(file);

				if( ! meshes[i].loadFromMs3dAsciiSegment(file) )
                {
                    bError = true;
                    break;
                }
			}
            continue;
		}

        //
        // materials
        //
        if (sscanf (szLine, "Materials: %d", &num_materials) == 1)
        {
            int i;

			materials = new MATERIAL[num_materials];

            for (i = 0; i < num_materials && !bError; i++)
            {
 				if( ! materials[i].loadFromMs3dAsciiSegment(file) )
                {
                    bError = true;
                    break;
                }
			}
            continue;
        }

		
        //
        // bones
        //
        bones = (BONES3D*)new BONES3D;
        bones->loadFromMs3dAsciiSegment( file, szLine );

    }

	fclose (file);

	if( !bones->link() )
		return false;
	bones->initialize();
	//attachSkin();
	//advanceAnimation( StartTime );
	return true;
}    
