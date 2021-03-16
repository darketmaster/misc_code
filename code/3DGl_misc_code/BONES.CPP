#include <stdlib.h>
#include "BONES.H" 
#include "filelog.h" 
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
////////////////////////   BONE 3D     //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
KEYFRAME::KEYFRAME()
{
  Time = 0;
  memset(Pos, 0, 3*sizeof(float) );
  memset(Rot, 0, 3*sizeof(float) );    
}    

KEYFRAME::~KEYFRAME()
{
}    
    	
void KEYFRAME::create(int time, float *p, float *r)
{
  Time = time;
  memcpy(Pos, p, 3*sizeof(float) );
  memcpy(Rot, r, 3*sizeof(float) );    
}    
/////////////////////////////////////////////////////////////////////////////
////////////////////////   BONE 3D     //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BONE3D::BONE3D()	
{
    clear();
}

void BONE3D::clear()
{
    memset(Name, 0, MS_MAX_NAME*sizeof(char) );
	memset(ParentName, 0, MS_MAX_NAME*sizeof(char) );
	Parent = NULL;
	memset(startPosition, 0, 3*sizeof(float) );
	memset(startRotation, 0, 3*sizeof(float) );
	m_relative = identity_matrix_f;
	m_final = identity_matrix_f;
	m_absolute = identity_matrix_f;
	NumKeys = 0;
	KeyFrames = NULL;
}

void  BONE3D::create(char *name, char *parentname, float *spos, float *srot, int nkeys, KeyFrame *keyp, KeyFrame *keyr)
{
    int i;
    
    KEYFRAME keytemp;   
    
    strcpy(Name, name);
    strcpy(ParentName, parentname);
    //memcpy(Name, name, MS_MAX_NAME*sizeof(char) );
	//memcpy(ParentName, parentname, MS_MAX_NAME*sizeof(char) );
	memcpy(startPosition, spos, 3*sizeof(float) );
	memcpy(startRotation, srot, 3*sizeof(float) );
	NumKeys = nkeys;
	KeyFrames = new KEYFRAME[NumKeys];
	for(i=0; i<NumKeys; i++)
	{
        keytemp.create((int)keyp[i].Time, keyp[i].Value, keyr[i].Value);
	    memcpy(&KeyFrames[i], &keytemp, sizeof(KEYFRAME) );
    }    
	
}    

BONE3D::~BONE3D()	
{
   destroy();
}

void BONE3D::destroy( void )
{
 	if(KeyFrames != NULL)
	{
		delete[] KeyFrames;
		KeyFrames = NULL;
		NumKeys = 0;
	}   
}

void BONE3D::render( void )
{
	V3D_f	vector;
	V3D_f	parentvector;

	apply_matrix_f(&m_final, 0, 0, 0, &vector.x, &vector.y, &vector.z);

	if( Parent != NULL )
	{
		apply_matrix_f(&Parent->m_final, 0, 0, 0, &parentvector.x, &parentvector.y, &parentvector.z);
	}

	glDisable( GL_TEXTURE_2D );

    if(Parent != NULL)
    {
	  // render bone as a line
	  glLineWidth(2.0);
	  glColor3f(0, 0, 1.0);
	  glBegin(GL_LINES);
	  glVertex3f( vector.x, vector.y, vector.z );
	  glVertex3f( parentvector.x, parentvector.y, parentvector.z );	
	  glEnd();
    }	

	// render bone-ends as fat points
	glPointSize(4.0);
	glColor3f(0, 1.0, 0);
	glBegin(GL_POINTS);
	glVertex3f( vector.x, vector.y, vector.z );
	if(Parent != NULL)
		glVertex3f( parentvector.x, parentvector.y, parentvector.z );	
	glEnd();
	glColor3f(1.0, 1.0, 1.0);

}

void BONE3D::initialize( void )
{
	// Create a transformation matrix from the position and rotation 
	get_transform_matrix_f(&m_relative, startRotation[0], startRotation[1], startRotation[2], 
                                        startPosition[0], startPosition[1], startPosition[2]);

	// Each bone's final matrix is its relative matrix concatenated onto its 
	// parent's final matrix (which in turn is ....)
	//
	if ( Parent == NULL )
	{
		m_final = m_relative;
		m_absolute = m_relative;
    }		
	else
	{
		matrix_mul_f(&m_relative, &Parent->m_absolute, &m_absolute);
        m_final = m_absolute;
	}
}

void BONE3D::advanceTo( float CurrentTime )
{
	int		i;
	float	deltaTime;
	float	fraction;
	float	Position[3];
	float	Rotation[3];
	MATRIX_f m_frame;

	/* Position */
	// Find appropriate position key frame
	i = 0;
	while( i < NumKeys-1 && KeyFrames[i].Time < CurrentTime )
		i++;

	if( i > 0 )
	{
		// Interpolate between 2 key frames
		// time between the 2 key frames
		deltaTime = KeyFrames[i].Time - KeyFrames[i-1].Time;
		// relative position of interpolation point to the keyframes [0..1]
		fraction = (CurrentTime - KeyFrames[i-1].Time) / deltaTime;

		Position[0] = KeyFrames[i-1].Pos[0] + fraction * (KeyFrames[i].Pos[0] - KeyFrames[i-1].Pos[0]);
		Position[1] = KeyFrames[i-1].Pos[1] + fraction * (KeyFrames[i].Pos[1] - KeyFrames[i-1].Pos[1]);
		Position[2] = KeyFrames[i-1].Pos[2] + fraction * (KeyFrames[i].Pos[2] - KeyFrames[i-1].Pos[2]);
		
#if 1
		QUAT q1,q2,q3;

        my_get_rotation_quat(&q1,(KeyFrames[i-1].Rot[0]),
                                 (KeyFrames[i-1].Rot[1]),
                                 (KeyFrames[i-1].Rot[2]));
        my_get_rotation_quat(&q2,(KeyFrames[i].Rot[0]),
                                 (KeyFrames[i].Rot[1]),
                                 (KeyFrames[i].Rot[2]));  
        my_quat_interpolate(&q1, &q2, fraction, &q3);                            
        my_quat_to_matrix(&q3, &m_frame);
#else

		Rotation[0] = KeyFrames[i-1].Rot[0] + fraction * (KeyFrames[i].Rot[0] - KeyFrames[i-1].Rot[0]);
		Rotation[1] = KeyFrames[i-1].Rot[1] + fraction * (KeyFrames[i].Rot[1] - KeyFrames[i-1].Rot[1]);
		Rotation[2] = KeyFrames[i-1].Rot[2] + fraction * (KeyFrames[i].Rot[2] - KeyFrames[i-1].Rot[2]);
		get_transform_matrix_f(&m_frame,(Rotation[0]), (Rotation[1]), (Rotation[2]),0,0,0); 
#endif 
        m_frame.t[0] = Position[0];
        m_frame.t[1] = Position[1];
        m_frame.t[2] = Position[2];   
	}
	else
	{
		memcpy(Position, KeyFrames[i].Pos, 3*sizeof(float) );
        memcpy(Rotation, KeyFrames[i].Rot, 3*sizeof(float) );
	}
	
	MATRIX_f m_fin;
	matrix_mul_f(&m_frame, &m_relative, &m_fin);	

	if ( Parent == NULL )					// this is the root node
		m_final = m_fin;
	else									// not the root node
		matrix_mul_f(&m_fin, &Parent->m_final, &m_final);
}

void BONE3D::debug( void )
{
    int i;

    add_in_log("BONE NAME:%s PARENTNAME:%s \n", Name, ParentName);
    add_in_log("startPosition (%f,%f,%f) startRotation (%f,%f,%f)\n", 
                startPosition[0], startPosition[1], startPosition[2],
                (startRotation[0]), (startRotation[1]), (startRotation[2]));       

    add_in_log("BONE NumKeys: %d\n", NumKeys);
    for(i=0;i<NumKeys;i++)
    {
        add_in_log("t: %d  pos(%f,%f,%f)  rot(%f,%f,%f)\n", KeyFrames[i].Time, 
                   KeyFrames[i].Pos[0], KeyFrames[i].Pos[1], KeyFrames[i].Pos[2],
                   KeyFrames[i].Rot[0], KeyFrames[i].Rot[1], KeyFrames[i].Rot[2]);
    }     
}

////text file operations////////////////////////////
int  BONE3D::save_txt_chunk(FILE *f)
{
    int i;
    
    if(!f) return 0;
    
    fprintf(f, "\"%s\"\n", Name );
    fprintf(f, "\"%s\"\n", ParentName );
    fprintf(f, "%f %f %f %f %f %f\n", startPosition[0], startPosition[1], startPosition[2],
                                      startRotation[0], startRotation[1], startRotation[2]);
    fprintf(f, "%d\n", NumKeys );
    for(i = 0; i < NumKeys; i++)
        fprintf(f, "%d %f %f %f %f %f %f\n", KeyFrames[i].Time, 
            KeyFrames[i].Pos[0], KeyFrames[i].Pos[1], KeyFrames[i].Pos[2],
            KeyFrames[i].Rot[0], KeyFrames[i].Rot[1], KeyFrames[i].Rot[2]);
    return 1;
} 

#define get_new_line(file) if(!fgets (szLine, 256, (file))){ return 0; }
   
int  BONE3D::load_txt_chunk(FILE *f)
{
    int i;
    char szLine[256];
    
    if(!f) return 0;
    
    get_new_line(f); 
    memset(Name, 0, MS_MAX_NAME*sizeof(char) );   
    if(sscanf (szLine, "\"%[^\"]\"", Name) != 1) return 0;
    
    get_new_line(f);
    memset(ParentName, 0, MS_MAX_NAME*sizeof(char) );  
    sscanf (szLine, "\"%[^\"]\"", ParentName);
    
    get_new_line(f);
    if(sscanf (szLine, "%f %f %f %f %f %f\n", &startPosition[0], &startPosition[1], &startPosition[2],
                                              &startRotation[0], &startRotation[1], &startRotation[2])!=6 )
       return 0;
       
    get_new_line(f);
    if(sscanf (szLine, "%d\n", &NumKeys )!=1) return 0;
    
    KeyFrames = new KEYFRAME[NumKeys];
    if(!KeyFrames) return 0;
    for(i = 0; i < NumKeys; i++)
    {
        get_new_line(f);
        sscanf (szLine, "%d %f %f %f %f %f %f\n", &KeyFrames[i].Time, 
            &KeyFrames[i].Pos[0], &KeyFrames[i].Pos[1], &KeyFrames[i].Pos[2],
            &KeyFrames[i].Rot[0], &KeyFrames[i].Rot[1], &KeyFrames[i].Rot[2]);
    }
    
    return 1;   
} 
///////binary file operations////////////
int  BONE3D::save_bin_chunk(FILE *f)
{
    if(!f) return 0;
    
    fwrite(Name, sizeof(char), MS_MAX_NAME, f);
    fwrite(ParentName, sizeof(char), MS_MAX_NAME, f);
    fwrite(startPosition, sizeof(float), 3, f);
    fwrite(startRotation, sizeof(float), 3, f);
    fwrite(&NumKeys, sizeof(int), 1, f);
    fwrite(KeyFrames, sizeof(KEYFRAME), NumKeys, f);
} 

int  BONE3D::load_bin_chunk(FILE *f)
{
    if(!f) return 0;
    
    fread(Name, sizeof(char), MS_MAX_NAME, f);    
    fread(ParentName, sizeof(char), MS_MAX_NAME, f);
    fread(startPosition, sizeof(float), 3, f);
    fread(startRotation, sizeof(float), 3, f);
    fread(&NumKeys, sizeof(int), 1, f);
    KeyFrames = new KEYFRAME[NumKeys];
    if(!KeyFrames) return 0;
    fread(KeyFrames, sizeof(KEYFRAME), NumKeys, f);
    
    return 1;   
}

int BONE3D::loadFromMs3dAsciiSegment( FILE *f )
{
    char szLine[256];
	int	 j;
	int	 nFlags;
	float ttim;
	int numkeys;

    // name
    get_new_line(f);
    if (sscanf (szLine, "\"%[^\"]\"", Name) != 1)return 0;

    // parent name
    get_new_line(f);
    strcpy (ParentName, "");
    sscanf (szLine, "\"%[^\"]\"", ParentName);

	get_new_line(f);
    if (sscanf (szLine, "%d %f %f %f %f %f %f",
                    &nFlags,
                    &startPosition[0], &startPosition[1], &startPosition[2],
                    &startRotation[0], &startRotation[1], &startRotation[2]) != 7)
		return 0;
 
 
    // position key count
    get_new_line(f);
    if (sscanf (szLine, "%d", &numkeys) != 1)return 0;
    
    NumKeys = numkeys;
	KeyFrames = new KEYFRAME[NumKeys];
    for (j = 0; j < NumKeys; j++)
    {
		get_new_line(f);
        if (sscanf (szLine, "%f %f %f %f", &ttim,&KeyFrames[j].Pos[0],&KeyFrames[j].Pos[1],&KeyFrames[j].Pos[2] ) != 4)
 			return 0;
		KeyFrames[j].Time = (int)ttim;
    }

    // rotation key count
    get_new_line(f);
    if (sscanf (szLine, "%d", &numkeys) != 1)return 0;

    for (j = 0; j < NumKeys; j++)
    {
		get_new_line(f);
        if (sscanf (szLine, "%f %f %f %f", &ttim,&KeyFrames[j].Rot[0],&KeyFrames[j].Rot[1],&KeyFrames[j].Rot[2] ) != 4)
  			return 0;
    }
	
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////   BONES 3D              ////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BONES3D::BONES3D()
{
    clear();
}

BONES3D::~BONES3D()
{
    destroy();
}        

void BONES3D::clear()
{
    NumFrames = 0;
    num_bones = 0;
    bones = NULL;
    num_atach = 0;
    atach = NULL;
} 

void BONES3D::destroy()
{
    if(bones)
    {
        delete [] bones;
    }    
    if(atach)
    {
        delete [] atach;
    }        
    clear();
}

bool BONES3D::link( void )
{
	// The relationship between child and parent bones are defined
	// by each child bone providing the name of its parent bone.
	// This function builds corresponding pointers for faster lookup.
	//
	int i, j;
	// Link children to parent
    for (i = 0; i < num_bones; i++)            
	{
		bones[i].Parent = NULL;
		if( strlen(bones[i].ParentName) >  0 )	// does bone have parent?
		{
			for (j = 0; j < num_bones; j++)	// search for parent
			{
				if( stricmp(bones[j].Name, bones[i].ParentName) == 0)	// j is parent of i
				{
					bones[i].Parent = &bones[j];
					break;
				}
			}
			if ( bones[i].Parent == NULL) // Unable to find parent bone 
				return false;
        }
    }
	
	return true;
}

void BONES3D::initialize( void )
{
	int i;

    for (i = 0; i < num_bones; i++)            
	{
		bones[i].initialize();
    }
}

void BONES3D::render( void )
{
	int	i;

	for(i = 0; i < num_bones; i++)	// for each bone
	{
		bones[i].render();	
	}
}

void BONES3D::render_atach( void )
{
	int	i;

	for(i = 0; i < num_bones; i++)	// for each bone
	{
		bones[i].render();	
	}
	if(atach)
	for(i = 0; i < num_atach; i++)	// for each bone
	{
	    if(atach[i]) 
		   atach[i]->render();	
	}
}

void BONES3D::advanceAnimation( float CurrentTime )
{
	int	i;

    for (i = 0; i < num_bones; i++)            
	{
		bones[i].advanceTo( CurrentTime );
	}
}

void BONES3D::advanceAnimation_atach( float CurrentTime )
{
	int	i;

    for (i = 0; i < num_bones; i++)            
	{
		bones[i].advanceTo( CurrentTime );
	}
	if(atach)
	for(i = 0; i < num_atach; i++)	// for each bone
	{
	    if(atach[i]) 
		   atach[i]->advanceAnimation_atach( CurrentTime );	
	}
}

void BONES3D::copy_from_model(Model *model)
{
    int i;
    int linkedok = 0;
    
    NumFrames = (int)model->MaxTime;
    num_bones = model->num_bones;

    bones = new BONE3D [num_bones];
    
    for(i = 0; i< num_bones; i++)
    {
        bones[i].create(model->bones[i].Name, 
                        model->bones[i].ParentName, 
                        model->bones[i].startPosition,
                        model->bones[i].startRotation,
                        model->bones[i].NumPositionKeys,
                        model->bones[i].PositionKeyFrames,
                        model->bones[i].RotationKeyFrames);
    }   
    linkedok = link();
    add_in_log("Link bones: %d\n", linkedok);
    initialize();  
}

void BONES3D::debug( void )
{
    int i;
    add_in_log("BONES3D....\n");
    add_in_log("NumFrames: %d\n", NumFrames);
    add_in_log("Numbones: %d\n", num_bones);
    for(i=0; i<num_bones; i++)
    {
        bones[i].debug();
    }
    add_in_log("Num atached bones: %d\n", num_atach); 
    for(i=0; i<num_atach; i++)
    {
        atach[i]->debug();
    }   
}   
/////////////////////////text file operations///////////////////////
int BONES3D::save_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wt");
    if(f)
    {
        signature_save_file_t(f,BONES3DHEADER);
        save_txt_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;    
}  
  
int BONES3D::load_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rt");
    if(f)
    {
        if(!signature_load_file_t(f,BONES3DHEADER))
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

int BONES3D::save_txt_chunk(FILE *f)
{
    int i;
    
    fprintf(f, "%d\n", NumFrames );   
    fprintf(f, "%d\n", num_bones ); 
    if(bones)
    for(i = 0; i < num_bones; i++)
        bones[i].save_txt_chunk(f);
        
    return 1;
}
    
int BONES3D::load_txt_chunk(FILE *f)   
{   
    char szLine[256]; 
    int i;
        
    get_new_line(f);    
    if(sscanf (szLine, "%d\n", &NumFrames )!=1) return 0;
    get_new_line(f);    
    if(sscanf (szLine, "%d\n", &num_bones )!=1) return 0;
    bones = new BONE3D [num_bones];
    if(bones) 
    for(i = 0; i < num_bones; i++)
        bones[i].load_txt_chunk(f);
    return 1;
}
///////////////////////////////////binary operations///////////////////////
int BONES3D::save_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wb");
    if(f)
    {
        signature_save_file_b(f, strlen(BONES3DHEADER), BONES3DHEADER);
        save_bin_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;    
}  
  
int BONES3D::load_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rb");
    if(f)
    {
        if(!signature_load_file_b(f, strlen(BONES3DHEADER), BONES3DHEADER))
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

int BONES3D::save_bin_chunk(FILE *f)
{
    int i;
    fwrite(&NumFrames, sizeof(int), 1, f); 
    fwrite(&num_bones, sizeof(int), 1, f); 
    if(bones)
    for(i = 0; i < num_bones; i++)
        bones[i].save_bin_chunk(f);   
    return 1;
}
    
int BONES3D::load_bin_chunk(FILE *f)   
{
    int i;
        
    fread(&NumFrames, sizeof(int), 1, f); 
    fread(&num_bones, sizeof(int), 1, f);
    bones = new BONE3D [num_bones];
    if(bones) 
    for(i = 0; i < num_bones; i++)
        bones[i].load_bin_chunk(f);
    return 1;
} 


int BONES3D::loadFromMs3dAsciiSegment( FILE *f , char *szLine)
{
  if (sscanf (szLine, "Bones: %d", &num_bones) == 1)
  {
      int i;

      bones = new BONE3D[num_bones];

      for (i = 0; i < num_bones; i++)
      {
		   if( ! bones[i].loadFromMs3dAsciiSegment(f) )
           {
              return 0;
           }
      }
  }
  return 1;     
}    









  
