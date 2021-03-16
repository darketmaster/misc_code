#include <string.h>
#include <ctype.h>
#include "log.h"
#include "models.h"
#include "mdf.h"

///////////////////////////////////////////////////////////////////////////
//                           CVector3 class
///////////////////////////////////////////////////////////////////////////

void CVector3::draw(CVector3 *inipos, float l)
{
     glBegin(GL_LINE_STRIP);
     glVertex3f(inipos->x, inipos->y, inipos->z);
     glVertex3f(inipos->x + x * l, inipos->y + y*l, inipos->z + z*l);
     glEnd ();
}

///////////////////////////////////////////////////////////////////////////
//                           _anim
///////////////////////////////////////////////////////////////////////////

_anim::_anim()
{ 
 name[0] = '\0'; 
 n_anim = 0;
 keyframe = NULL;
}

_anim::_anim(char *name,int tam)
{
  create(name, tam);
} 

_anim::~_anim()
{
  destroy();
}
 
void _anim::destroy()
{
   if(n_anim>0) 
      if(keyframe) 
         delete [] keyframe;
   name[0] = '\0'; 
   n_anim = 0;
   keyframe = NULL;
}
 
void _anim::create(char *name,int tam)
{
 strcpy(this->name,name); 
 n_anim = tam;
 if(n_anim>0)
    keyframe = (short*)new short[n_anim];
 else keyframe = NULL;
}
 
void _anim::set(int n, short data)
{
   if((n_anim>0) && (n<n_anim))
      if(keyframe) keyframe[n] = data;           
}

void _anim::load(FILE *fp)
{
    char line[256];
    char *p;
    int stop=0;
    int c,n;
    char tname[64] = "";
    
   	p = fgets(line, sizeof(line), fp);
	while(p != NULL && !stop)
	{

		if(strncmp(p, "NAME= \"", strlen("NAME= \"")) == 0 )
		{
			p+=7;
			c=0;
			while(*p != '\n' && *p != '"' && c<63)
			{
			   tname[c] = *p;
			   p++;
			   c++;
			}			
			tname[c] = '\0';			
		}
		else		
		if(strncmp(p, "N_FRAMES= ", strlen("N_FRAMES= ")) == 0 )
		{
			p+=10;
			while(*p == ' ') p++;			
			if(isdigit(*p))
            {			
			   sscanf(p, "%d", &n);			   
            }
            if (n<0) n = 0;			
            create(tname,n);
		}
		else
		if(strncmp(p, "LIST_BEGIN.", strlen("LIST_BEGIN.")) == 0 )
		{
		   int l = strlen("LIST_END.");		   
		   p = fgets(line, sizeof(line), fp);
		   c = 0;  //add_in_log("LIST_BEGIN localized\n");
           while(p != NULL && strncmp(p, "LIST_END.", l))
           {
              
              while(*p != '\n' && c < n_anim)
              {
                while(*p == ' ') p++;
                
                if(isdigit(*p))
                {
                   sscanf(p, "%d", &n);   //add_in_log("line p: %s\n", p); //add_in_log("scan data %d: %d\n",c,n);
                   set(c, (short)n);
                   c++;
                }
                while(*p != ' ' && *p != '\n') p++;
                
              }
              p = fgets(line, sizeof(line), fp);              
           }   //if(strncmp(p, "LIST_END.", l)==0) add_in_log("LIST_END localized\n");           
           stop = 1;
		}
	    p = fgets(line, sizeof(line), fp);	
	}
}

void _anim::save(FILE *fp)
{
  int c;
  fprintf(fp,"NAME= \"%s\"\n", name);
  fprintf(fp,"N_FRAMES= %d\n", n_anim);
  
  fprintf(fp,"LIST_BEGIN.\n"); 
  for(c=0; c<n_anim; c++)
  {
    fprintf(fp,"%d ", keyframe[c]); 
  }
  fprintf(fp,"\nLIST_END.\n\n");     
}

void _anim::debug()
{
  int c;
  add_in_log("ANIMATION NAME [%s]\n", name);
  add_in_log("n_anim: %d\n", n_anim);
  
  add_in_log("keyframe list %X:\n", keyframe); 
  for(c=0; c<n_anim; c++)
  {
    add_in_log("%d ", keyframe[c]); 
  }
  add_in_log("\n");           
}

///////////////////////////////////////////////////////////////////////////
//                           ANIMATION
///////////////////////////////////////////////////////////////////////////
ANIMATION::ANIMATION()
{
  n_animations = 0; 
  animation = NULL;
}

ANIMATION::~ANIMATION()
{   
   if(n_animations>0) 
      if(animation) 
         free(animation);//delete [] animation; //i use free for use realloc
   n_animations = 0;
   animation = NULL;
}

void ANIMATION::load(char *filename)
{
    char line[256];
    char *p;
    FILE *fp;
    
    fp = fopen(filename,"rt");
    
    if(fp)
    {
    
   	   p = fgets(line, sizeof(line), fp);
	   while(p != NULL)
	   {
		 if(strncmp(p, "ANIMATION.", strlen("ANIMATION.")) == 0 )
		 {
		    animation = (_anim*) realloc(animation, sizeof(_anim)*(n_animations+1));
            animation[n_animations].load(fp);
            n_animations++;
		 }
	     p = fgets(line, sizeof(line), fp);	
	   }
	   fclose(fp);
   }//end if
}

void ANIMATION::save(char *filename)
{
    FILE *fp;
    int c;
    
    fp = fopen(filename,"wt");
    
    if(fp)
    {
		    for( c=0; c<n_animations; c++)
		    {
		       fprintf(fp, "ANIMATION.\n");
		       animation[c].save(fp);
            }
	   fclose(fp);
    }//end if
}


///////////////////////////////////////////////////////////////////////////
//                           STANIMATION
///////////////////////////////////////////////////////////////////////////

STANIMATION::STANIMATION()
{
 velanim = 1000;
 anim_per = 1000.0f / (float(velanim)); 
 pinfo = NULL; 
 canim = -1; 
 currkanim = 0;
 nextkanim = 0; 
 elapsedTime = 0.0f;
 lastTime = 0.0f;
}

STANIMATION::~STANIMATION()
{
}
 
void STANIMATION::create(int _velanim, ANIMATION *_pinfo)
{
 velanim = _velanim;
 anim_per = 1000.0f / (float(velanim)); 
 pinfo = _pinfo; 
 canim = 0; 
 currkanim = 0;
 nextkanim = 0; 
 elapsedTime = 0.0f;
 lastTime = 0.0f;
} 

void STANIMATION::set_vel(int _velanim)
{
 velanim = _velanim;
 anim_per = 1000.0f / (float(velanim)); 
}

void STANIMATION::set_canim(int _canim)
{
 if(_canim < pinfo->n_animations)
    canim = _canim;
 else canim = 0;
 currkanim = 0;
 nextkanim = 0; 
}

float STANIMATION::Animate()
{

	float time,t;
	
    time = GetTickCount(); 
    nextkanim = (currkanim + 1) % pinfo->animation[canim].n_anim;	
	elapsedTime = time - lastTime;
    t = elapsedTime / anim_per; 
     
	if (elapsedTime >= anim_per )
	{
		currkanim = nextkanim;
		lastTime = time;
	}    
	return t;
}

int STANIMATION::get_ckanim()
{ 
   int ret = 0;
   if(pinfo) if(pinfo->animation[canim].keyframe)
        ret = (int)pinfo->animation[canim].keyframe[currkanim];
   return ret;
}

int STANIMATION::get_nkanim()
{
   int ret = 0;
   if(pinfo)  if(pinfo->animation[canim].keyframe)
        ret = (int)pinfo->animation[canim].keyframe[nextkanim];
   return ret;
}


///////////////////////////////////////////////////////////////////////////
//                          make_texture
///////////////////////////////////////////////////////////////////////////

GLuint make_texture(char *filename, int type, int masked)
{
	PALETTE pal;
	BITMAP *bmp;
	GLuint tex;
	int usemipmap;
	
	usemipmap = allegro_gl_use_mipmapping(FALSE); //get the mipmap state
    allegro_gl_use_mipmapping(usemipmap);         //return to old state

	bmp = load_bitmap (filename, pal);

	allegro_gl_begin();
	
	if(type>TLOW) glEnable (GL_DITHER);
	else glDisable (GL_DITHER);
	
	glEnable (GL_TEXTURE_2D);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    if(!masked)
	   tex = allegro_gl_make_texture(bmp);
	else tex = allegro_gl_make_masked_texture(bmp);
	
	switch(type)
	{
	   case TLOW:
	        if(usemipmap)
            {
   	        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
	        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	        }
	        else
	        {
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
	   break;
	   case TNORM:
            if(usemipmap)
            {
	           glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	           glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	        }
	        else
	        {
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
	   break;
	   case THIGH:
            if(usemipmap)
            {
   	        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	        }
	        else
	        {
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }	        
	   break;
	}
	
	allegro_gl_end();	
	add_in_log("loading texture file %s tex=%d\n",filename,tex);  

	destroy_bitmap (bmp);
	return tex;
}

///////////////////////////////////////////////////////////////////////////
//                          misc functions
///////////////////////////////////////////////////////////////////////////
/*
void draw_t3DModel(t3DModel *g_3DModel, int g_ViewMode, UINT g_Texture[])
{
	for(int i = 0; i < g_3DModel->numOfObjects; i++)
	{
		if(g_3DModel->pObject.size() <= 0) break;
		
		t3DObject *pObject = &g_3DModel->pObject[i];
		
		if(pObject->bHasTexture) {
			glEnable(GL_TEXTURE_2D);
			glColor3ub(255, 255, 255);
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} else {
			glDisable(GL_TEXTURE_2D);
			glColor3ub(255, 255, 255);
		}
		
		glBegin(g_ViewMode);
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					int index = pObject->pFaces[j].vertIndex[whichVertex];
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
					if(pObject->bHasTexture) {
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} else {
						if(g_3DModel->pMaterials.size() && pObject->materialID >= 0) 
						{
							BYTE *pColor = g_3DModel->pMaterials[pObject->materialID].color;
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
					}
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				}
			}
		glEnd();
	}
}

void set_textures_g_3DModel(t3DModel *g_3DModel,UINT g_Texture[], char *PATHTEXTURES3DS)
{
  char filename[256];
	for(int i = 0; i < g_3DModel->numOfMaterials; i++)
	{
		if(strlen(g_3DModel->pMaterials[i].strFile) > 0)
		{
		   sprintf(filename,"%s/%s",PATHTEXTURES3DS,g_3DModel->pMaterials[i].strFile);
		   g_Texture[i] = make_texture(filename, TNORM, TNOMASKED);
		}
		g_3DModel->pMaterials[i].texureId = i;
		add_in_log("g_3DModel->pMaterials[%d].texureId = %d\n", i,i);
	}
}

void debug_g_3DModel(t3DModel *g_3DModel)
{
  char filename[256];
  
  if(g_3DModel)
  {
  add_in_log("NUMMATERIASL: %d\n",g_3DModel->numOfMaterials);
  add_in_log("NUMOBJECTS: %d\n",g_3DModel->numOfObjects);
  
  for(int i = 0; i < g_3DModel->numOfMaterials; i++)
  {
	  if(strlen(g_3DModel->pMaterials[i].strFile) > 0)
	     add_in_log("texture[%d]: %s\n",i,g_3DModel->pMaterials[i].strFile);
	  else 
		 add_in_log("texture[%d]: notexture\n",i);
   }
	
	
	if(g_3DModel->pObject.size() > 0)
	for(int i = 0; i < g_3DModel->numOfObjects; i++)
	{
       add_in_log("hastexture: %d\n",g_3DModel->pObject[i].bHasTexture);
       add_in_log("NUMFACES: %d\n",g_3DModel->pObject[i].numOfFaces);
    }
 }

}
*/
///////////////////////////////////////////////////////////////////////////
//                           BBOX
///////////////////////////////////////////////////////////////////////////
BBOX::BBOX(CVector3 *_min,CVector3 *_max)
{ 
  color[0] = color[1] = color[2] = 255;
  min = *_min;
  max = *_max;
}

BBOX::BBOX(CVector3 *_min,CVector3 *_max, GLubyte col[])
{ 
   init(_min, _max, col);
}

void BBOX::init(CVector3 *_min,CVector3 *_max, GLubyte col[]) 
{ 
   min = *_min;
   max = *_max;
   color[0] = col[0];
   color[1] = col[1];
   color[2] = col[2];
}

void BBOX::render()
{

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
    
  glColor3ub(color[0], color[1], color[2]);  
  glLineWidth (1.0f); 
  
  glBegin(GL_LINE_LOOP);
  glVertex3f( min.x, min.y, min.z );
  glVertex3f( min.x, max.y, min.z );
  glVertex3f( min.x, max.y, max.z );
  glVertex3f( min.x, min.y, max.z );
  glEnd();
  _polycount++;
  
  glBegin(GL_LINE_LOOP);
  glVertex3f( max.x, min.y, min.z );
  glVertex3f( max.x, max.y, min.z );
  glVertex3f( max.x, max.y, max.z );
  glVertex3f( max.x, min.y, max.z );
  glEnd();
  _polycount++;
  
  glBegin(GL_LINE_STRIP);
  glVertex3f( min.x, max.y, min.z );
  glVertex3f( max.x, max.y, min.z );
  glEnd();
  
  glBegin(GL_LINE_STRIP);
  glVertex3f( max.x, max.y, max.z );
  glVertex3f( min.x, max.y, max.z );
  glEnd();
  
  glBegin(GL_LINE_STRIP);
  glVertex3f( min.x, min.y, min.z );
  glVertex3f( max.x, min.y, min.z );
  glEnd();
  
  glBegin(GL_LINE_STRIP);
  glVertex3f( max.x, min.y, max.z );
  glVertex3f( min.x, min.y, max.z );
  glEnd(); 
  
  glEnable(GL_TEXTURE_2D); 
  glEnable(GL_CULL_FACE);  
}


///////////////////////////////////////////////////////////////////////////
//                           frame_obj3d
///////////////////////////////////////////////////////////////////////////

frame_obj3d::frame_obj3d()
{
   numVertices = 0;
   pVerts = NULL;
   pNormals = NULL;
}

frame_obj3d::~frame_obj3d()
{
  destroy();
}

void frame_obj3d::create( int nv)
{
   int i;
   destroy();
   
   numVertices = nv;
   pVerts = (CVector3*)new CVector3[numVertices];
   pNormals = (CVector3*)new CVector3[numVertices];   
}

void frame_obj3d::destroy()
{
  int i;
  if(numVertices>0)
  {
     if(pVerts)   { delete [] pVerts;  }
     if(pNormals)   { delete [] pNormals;  }
  }
   numVertices = 0;
   pVerts = NULL;
   pNormals = NULL;
}

void frame_obj3d::scale(float _scale)
{
  int i, c;
  bbox.scale(_scale);
  if(pVerts)
     for(i=0;i<numVertices;i++)
         pVerts[i] *= _scale;
};

void frame_obj3d::save_data_mdf(FILE *fp)
{
  fwrite(&numVertices, sizeof(int)     , 1          , fp);
  fwrite(pVerts      , sizeof(CVector3), numVertices, fp);
  //fwrite(pNormals    , sizeof(CVector3), numVertices, fp);
  fwrite(&bbox       , sizeof(_bbox)   , 1          , fp);
}

void frame_obj3d::load_data_mdf(FILE *fp)
{
  fread(&numVertices, sizeof(int)     , 1          , fp);
  fread(pVerts      , sizeof(CVector3), numVertices, fp);
  //fread(pNormals    , sizeof(CVector3), numVertices, fp);  
  fread(&bbox       , sizeof(_bbox)   , 1          , fp);
}


///////////////////////////////////////////////////////////////////////////
//                           _obj3d
///////////////////////////////////////////////////////////////////////////

_obj3d::_obj3d()
{
   numFrames = 0; 
   data = NULL;
   numTexCoords = 0; 
   texcoord = NULL;
   numTriangles = 0;
   pTri = NULL;  
   pVtemp = NULL;
}

_obj3d::~_obj3d()
{
  destroy();
}
   
void _obj3d::create(int nf, int nt, int ntr)
{  
   numFrames = nf;
   data = (frame_obj3d*)new frame_obj3d[numFrames];
   numTexCoords = nt; 
   texcoord = (CVector2*)new CVector2[numTexCoords];   
   numTriangles = ntr; 
   pTri = (tri_mda*)new tri_mda[numTriangles];  
}

void _obj3d::destroy()
{
   int i;
   if(numFrames > 0)
      if(data)
      {
         for(i=0; i<numFrames ;i++)
             data[i].destroy();
         delete [] data;
      }
   if(numTexCoords > 0)
      if(texcoord) {  delete [] texcoord; }
   if(numTriangles > 0)
      if(pTri) {  delete [] pTri; }
      
   if(pVtemp) delete [] pVtemp;

   numFrames = 0;
   data = NULL;
   numTexCoords = 0; 
   texcoord = NULL;
   numTriangles = 0;
   pTri = NULL;  
   pVtemp = NULL;
}

void _obj3d::set_tri(int n, tri_mda *tri)
{

  if( n<numTriangles )
     if(pTri)
     {
        pTri[n] = *tri;
     }
}
/*
void _obj3d::load(char *filename)
{
   int i,j,c;
   float x,y,z;
   int vt[3];
   int tind[3];
   int cframe;
   tri_mda ttri;
   CVector3 min,max; //for bbox
   GLubyte color[3] = {255,255,255};
   
   add_in_log("loading_model \"%s\"\n", filename);            
   MD2MODEL md2mod(filename);
   
   if(md2mod.numVertices>0)
   {
      add_in_log("creating Model3D_mda whit %d frames %d TexCoords num_vert:%d numpolys:%d\n", md2mod.numFrames, md2mod.numTexCoords,md2mod.numVertices,md2mod.numTriangles); 
      create(md2mod.numFrames,md2mod.numTexCoords,md2mod.numTriangles);
      pVtemp = (CVector3*)new CVector3[md2mod.numVertices];
      
      add_in_log("Model3D_mda whit frames: %d,  TexCoords: %d, numTriangles: %d\n", numFrames, numTexCoords, numTriangles); 
      
      if(md2mod.skinWidth!=0)
         x = float(md2mod.skinWidth);
      else x = 1;            
            
      if(md2mod.skinHeight!=0)
         y = float(md2mod.skinHeight);
      else y = 1; 
      
      if(texcoord)                
      for(c=0; c<numTexCoords; c++)
      {
          texcoord[c].x = float(md2mod.texturecoord[c].s) / x;
          texcoord[c].y = 1.0 - float(md2mod.texturecoord[c].t) / y;
      }      
      add_in_log("TexCoords...ok\n"); 
   
      for(cframe = 0; cframe< md2mod.numFrames; cframe++)
      {
		 CVector3 *pNormals		= (CVector3*)new CVector3 [md2mod.numTriangles];
		 CVector3 *pTempNormals	= (CVector3*)new CVector3 [md2mod.numTriangles];
      
         data[cframe].create(md2mod.numVertices);
         min.init(0.0f,0.0f,0.0f);
         max.init(0.0f,0.0f,0.0f);
     
         for(i=0;i<md2mod.numTriangles;i++)
         {
            vt[0] = (int) md2mod.triangles[i].vertexIndices[0];
            vt[1] = (int) md2mod.triangles[i].vertexIndices[1];
            vt[2] = (int) md2mod.triangles[i].vertexIndices[2];
            tind[0] = (int) md2mod.triangles[i].textureIndices[0];
            tind[1] = (int) md2mod.triangles[i].textureIndices[1];
            tind[2] = (int) md2mod.triangles[i].textureIndices[2];            
        
            for(c=0;c<3;c++)
            {
              ttri.v[c] = vt[c];
              ttri.uv[c] = tind[c];
              
              x = ((float)md2mod.frames[cframe].vertices[vt[c]].vertex[0] * md2mod.frames[cframe].scale[0]) + md2mod.frames[cframe].translate[0];
              z = ((float)md2mod.frames[cframe].vertices[vt[c]].vertex[1] * md2mod.frames[cframe].scale[1]) + md2mod.frames[cframe].translate[1];
              y = ((float)md2mod.frames[cframe].vertices[vt[c]].vertex[2] * md2mod.frames[cframe].scale[2]) + md2mod.frames[cframe].translate[2];
              
              if(x<min.x)min.x=x;              
              if(y<min.y)min.y=y;
              if(z<min.z)min.z=z;
              
              if(x>max.x)max.x=x;              
              if(y>max.y)max.y=y;
              if(z>max.z)max.z=z;
                       
              data[cframe].pVerts[vt[c]].init(x, y, z);             
            }//end for c
            
            if(cframe==0)
               set_tri(i, &ttri);
       
            CVector3 vVector1, vVector2, vNormal;

            vVector1 = data[cframe].pVerts[vt[0]] - data[cframe].pVerts[vt[2]];
            vVector2 = data[cframe].pVerts[vt[2]] - data[cframe].pVerts[vt[1]];	

			vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
            pTempNormals[i] = vNormal;			
 			vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal
			pNormals[i] = vNormal;						// Assign the normal to the list of normals
 
        }//end for i
        
        //////////////// Now Get The Vertex Normals /////////////////
		CVector3 vSum(0.0, 0.0, 0.0);
		CVector3 vZero = vSum;
		int shared=0;

		for (i = 0; i < data[cframe].numVertices; i++)
		{
			for (j = 0; j < numTriangles; j++)
			{	// Check if the vertex is shared by another face
				if (pTri[j].v[0] == i || pTri[j].v[1] == i || 
					pTri[j].v[2] == i)
				{
					vSum = vSum + pTempNormals[j];		// Add the un-normalized normal of the shared face
					shared++;	// Increase the number of shared triangles
				}
			}      
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			data[cframe].pNormals[i] = DivideVectorByScaler(vSum, float(-shared));
			data[cframe].pNormals[i] = Normalize(data[cframe].pNormals[i]);	//pNormals[i].unitize();
			vSum = vZero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
        
        delete [] pTempNormals;
		delete [] pNormals;
		
		data[cframe].bbox.init(&min,&max);
		
     }//end for cframe
     
     tbbox.init(&min,&max,color);
     
  }//end if 
  
}
*/
void _obj3d::render_interpoled(int cframe, int nframe, float t)
{   
  calculate_interpoled(cframe, nframe, t);
  render();
}

//warning:all values are valids ; p(t) = p0 + t(p1 - p0) is used
void _obj3d::calculate_interpoled(int cframe, int nframe, float t)
{
  int i, c;
  CVector3 *v1,*v2;
  CVector3 vnorm;
  
  tbbox.min = data[cframe].bbox.min + (data[nframe].bbox.min - data[cframe].bbox.min) * t; 
  tbbox.max = data[cframe].bbox.max + (data[nframe].bbox.max - data[cframe].bbox.max) * t;
  
  for(i=0;i<data[0].numVertices;i++)
  {
      v1 = &data[cframe].pVerts[i];
      v2 = &data[nframe].pVerts[i];   
      pVtemp[i].x = v1->x + (v2->x - v1->x) * t;  
      pVtemp[i].y = v1->y + (v2->y - v1->y) * t;  
      pVtemp[i].z = v1->z + (v2->z - v1->z) * t; 
  }
}

void _obj3d::scale(float _scale)
{
  int i;
  if(data)
    for(i=0;i<numFrames;i++)
    {  
       data[i].scale(_scale);
    }
}

//warning:all values are valids ; p(t) = p0 + t(p1 - p0) is used
void _obj3d::render_outline()
{
  int i, c;
  CVector3 *vtemp;
  CVector3 *vnorm;
  
//  glHint (GL_LINE_SMOOTH_HINT, GL_FASTEST);
//  glEnable (GL_LINE_SMOOTH);  
  glDisable (GL_TEXTURE_2D);  
//  glEnable (GL_BLEND);
//  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_BACK, GL_LINE);  	
  glCullFace (GL_FRONT);  
  glDepthFunc (GL_LEQUAL);  		
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth (3.0f);  
  glBegin(GL_TRIANGLES);  
  for(i=0;i<numTriangles;i++)
  { 
    for(c=0;c<3;c++) 
    { 
      vtemp = &pVtemp[pTri[i].v[c]];      
      glVertex3f( vtemp->x,vtemp->y, vtemp->z);
     } 
     _polycount++;    
  }
  glEnd (); 
 glDepthFunc (GL_LESS); 
 glPolygonMode (GL_BACK, GL_FILL);
 glCullFace (GL_BACK);
 glDisable (GL_BLEND); 
}


void _obj3d::render()
{
  int i, c;
  CVector3 *vtemp;
  CVector3 *vnorm;
  
  for(i=0;i<numTriangles;i++)
  {
    for(c=0;c<3;c++) 
    {
      vnorm = &data[0].pNormals[pTri[i].v[c]];   
      vtemp = &pVtemp[pTri[i].v[c]];  
      
      glNormal3f(vnorm->x,vnorm->y, vnorm->z);      
      glTexCoord2f(texcoord[pTri[i].uv[c]].x, texcoord[pTri[i].uv[c]].y);      
      glVertex3f( vtemp->x,vtemp->y, vtemp->z);
     }
     _polycount++;
  }
  glEnd ();
}

void _obj3d::load_mdf(char *filename)
{
  int sign;                 //MDFSIG
  int offsetTexCoords;
  int offsetTriangles;
  int offsetFrames;
  FILE *fp;
  
  fp = fopen(filename,"rb");
  
  if(fp)  
  {
    fread(&sign        , sizeof(int) , 1  , fp);    
    fread(&numTriangles, sizeof(int) , 1  , fp);
    fread(&numTexCoords, sizeof(int) , 1  , fp);
    fread(&numFrames   , sizeof(int) , 1  , fp);
    
    add_in_log("SIGN :%d\n", sign);
    add_in_log("numTriangles :%d\n", numTriangles);
    add_in_log("numTexCoords :%d\n", numTexCoords);
    add_in_log("numFrames :%d\n"   , numFrames);  
  
    offsetTriangles = sizeof(int)*7;
    offsetTexCoords = sizeof(tri_mda)*numTriangles + offsetTriangles;
    offsetFrames += offsetTexCoords;
    
    fread(&offsetTriangles, sizeof(int) , 1 , fp);
    fread(&offsetTexCoords, sizeof(int) , 1 , fp);
    fread(&offsetFrames   , sizeof(int) , 1 , fp);
    
    add_in_log("offsetTriangles :%d\n", offsetTriangles);
    add_in_log("offsetTexCoords :%d\n", offsetTexCoords); 
    add_in_log("offsetFrames :%d\n"   , offsetFrames); 
    
    
    fclose(fp);
  }//end if(fp) 
}

void _obj3d::save_mdf(char *filename)
{
  int sign = MDFSIG;
  int offsetTexCoords;
  int offsetTriangles;
  int offsetFrames;
  FILE *fp;
  
  fp = fopen(filename,"wb");
  
  offsetFrames  = sizeof(int);
  offsetFrames += sizeof(CVector3)*data[0].numVertices;//*2;
  offsetFrames += sizeof(_bbox);
  
  if(fp)  
  {
    fwrite(&sign        , sizeof(int) , 1  , fp);    
    fwrite(&numTriangles, sizeof(int) , 1  , fp);
    fwrite(&numTexCoords, sizeof(int) , 1  , fp);
    fwrite(&numFrames   , sizeof(int) , 1  , fp);
    
    add_in_log("SIGN :%X\n", sign);
    add_in_log("numTriangles :%d\n", numTriangles);
    add_in_log("numTexCoords :%d\n", numTexCoords);
    add_in_log("numFrames :%d\n"   , numFrames);  
  
    offsetTriangles = sizeof(int)*7;
    offsetTexCoords = sizeof(tri_mda)*numTriangles + offsetTriangles;
    offsetFrames += offsetTexCoords;
    
    fwrite(&offsetTriangles, sizeof(int) , 1 , fp);
    fwrite(&offsetTexCoords, sizeof(int) , 1 , fp);
    fwrite(&offsetFrames   , sizeof(int) , 1 , fp); 
    
    add_in_log("offsetTriangles :%d\n", offsetTriangles);
    add_in_log("offsetTexCoords :%d\n", offsetTexCoords); 
    add_in_log("offsetFrames :%d\n"   , offsetFrames); 
    
    fwrite(pTri     , sizeof(tri_mda)     , numTriangles , fp);
    fwrite(texcoord , sizeof(CVector2) , numTexCoords , fp);    
    
    for(int cframe = 0; cframe<numFrames; cframe++)
        data[cframe].save_data_mdf(fp);
        
    
    fclose(fp);
  }//end if(fp) 
}

///////////////////////////////////////////////////////////////////////////
//                           GLMATERIAL
///////////////////////////////////////////////////////////////////////////
GLMATERIAL::GLMATERIAL()
{
   cur = -1;
   max = 0;
   tex = NULL;
}

GLMATERIAL::~GLMATERIAL()
{
  destroy();
}

GLMATERIAL::GLMATERIAL(int n)
{
   create(n);
}
   
void GLMATERIAL::create(int n)
{
   
   cur = -1;
   if(n<MAX_NUM_TEXTURES)
      max = n;
   else max = MAX_NUM_TEXTURES;
   tex = (GLuint *)new GLuint[max];
}

void GLMATERIAL::destroy()
{
 int i;
 
   if(cur >= 0)
   {
      if(tex)
      for(i=0; i<=cur; i++)
          glDeleteTextures(cur+1, tex);
   }
   
   if(tex) delete [] tex;
      
   cur = -1;
   max = 0;
   tex = NULL;
}

int GLMATERIAL::add(char *filename, int type, int masked)
{
  int ttex;
  if(cur < max-1)
  {
     cur++;
     ttex = make_texture(filename, type, masked);
     tex[cur] = ttex;
  }
  return cur; 
}

int GLMATERIAL::replace(int n, char *filename, int type, int masked)
{
  int ttex;
  int tid;
  if(n <= cur)
  {
     glDeleteTextures(1, &tex[n]);
     ttex = make_texture(filename, type, masked);
     tex[n] = ttex;
     tid = n;
  }
  else tid = add(filename, type, masked);
  
  return tid; 
}

/////////////////////////////////////////////////////////////////////////////
//               MDF_OBJECT
/////////////////////////////////////////////////////////////////////////////
MDF_OBJ::MDF_OBJ()
{
  blank();
  blank_mdf_obj();
}

MDF_OBJ::~MDF_OBJ()
{
  destroy();
  destroy_mdf_obj();
  
}  

void MDF_OBJ::blank_mdf_obj()
{
  pTVerts   = NULL;
  pTNormals = NULL;
  bbox      = NULL;
  pTVNormals = NULL;
}

void MDF_OBJ::create_mdf_obj()
{
  pTVerts   = (CVector3*) new CVector3[numVertices];
  pTNormals = (CVector3*) new CVector3[numTriangles];
  pTVNormals = (CVector3*) new CVector3[numVertices];
  bbox      = (_bbox*) new _bbox[numFrames];
  tbbox.color[0] = tbbox.color[1] = tbbox.color[2] = 255;
}
  
void MDF_OBJ::import_md2(char *filename)
{
   load_md2(filename);
   create_mdf_obj();
   calculate_bbox();   
}

void MDF_OBJ::import_mdf(char *filename)
{
   load_mdf(filename);
   create_mdf_obj();
   calculate_bbox();
}

void MDF_OBJ::destroy_mdf_obj()
{
  if(pTVerts)   delete [] pTVerts;
  if(pTNormals) delete [] pTNormals;
  if(pTVNormals) delete [] pTVNormals;  
  if(bbox)      delete [] bbox;
  blank_mdf_obj();
}

void MDF_OBJ::debug_mdf_obj()
{
   debug();
   add_in_log("Bbox: %X\n",bbox); 
   for(int i=0; i<numFrames; i++)
   {
     add_in_log("[%03d]: (%.4f,%.4f,%.4f) \n", i ,
                bbox[i].min.x,bbox[i].min.y,bbox[i].min.z,
                bbox[i].max.x,bbox[i].max.y,bbox[i].max.z );
   }
}

void MDF_OBJ::calculate_bbox()
{
   float x,y,z;
   int vt[3];
   int cframe;
   CVector3 min,max; //for bbox
   
  for(cframe = 0; cframe< numFrames; cframe++)
  {
     min.init(0.0f,0.0f,0.0f);
     max.init(0.0f,0.0f,0.0f);
     for(int i=0;i<numTriangles;i++)
     {
         vt[0] = (int) triangles[i].v[0];
         vt[1] = (int) triangles[i].v[1];
         vt[2] = (int) triangles[i].v[2];
         for(int c=0;c<3;c++)
         {              
         x = ((float)frames[cframe].vertices[vt[c]].v[0] * frames[cframe].scale.x) + frames[cframe].translate.x;
         z = ((float)frames[cframe].vertices[vt[c]].v[1] * frames[cframe].scale.y) + frames[cframe].translate.y;
         y = ((float)frames[cframe].vertices[vt[c]].v[2] * frames[cframe].scale.z) + frames[cframe].translate.z;
         if(x<min.x)min.x=x;              
         if(y<min.y)min.y=y;
         if(z<min.z)min.z=z;              
         if(x>max.x)max.x=x;              
         if(y>max.y)max.y=y;
         if(z>max.z)max.z=z;                    
         }//end for c
         bbox[cframe].init(&min,&max);		
     }//end for i
  }//end for cframe
}//end func

//warning:all values are valids ; p(t) = p0 + t(p1 - p0) is used
void MDF_OBJ::calculate_interpoled(int cframe, int nframe, float t, int smooth)
{
  int i, j, c;
  CVector3 v1,v2,v[3];
  CVector3 vnorm;
  float x,y,z;
  int vt[3];
  CVector3 vSum(0.0, 0.0, 0.0);
  CVector3 vZero = vSum;
  int shared=0;
  
  CVector3 *pTempNormals	= (CVector3*)new CVector3 [numTriangles];
  
  tbbox.min = bbox[cframe].min + (bbox[nframe].min - bbox[cframe].min) * t; 
  tbbox.max = bbox[cframe].max + (bbox[nframe].max - bbox[cframe].max) * t;
   
  for(int i=0;i<numTriangles;i++)
  {
     vt[0] = (int) triangles[i].v[0];
     vt[1] = (int) triangles[i].v[1];
     vt[2] = (int) triangles[i].v[2];
     
     for(int c=0;c<3;c++)
     {              
       v1.x = ((float)frames[cframe].vertices[vt[c]].v[0] * frames[cframe].scale.x) + frames[cframe].translate.x;
       v1.z = ((float)frames[cframe].vertices[vt[c]].v[1] * frames[cframe].scale.y) + frames[cframe].translate.y;
       v1.y = ((float)frames[cframe].vertices[vt[c]].v[2] * frames[cframe].scale.z) + frames[cframe].translate.z;
       v2.x = ((float)frames[nframe].vertices[vt[c]].v[0] * frames[nframe].scale.x) + frames[nframe].translate.x;
       v2.z = ((float)frames[nframe].vertices[vt[c]].v[1] * frames[nframe].scale.y) + frames[nframe].translate.y;
       v2.y = ((float)frames[nframe].vertices[vt[c]].v[2] * frames[nframe].scale.z) + frames[nframe].translate.z;
       
       v[c] = v1+(v2-v1)*t;       
     }//end for c
     
     for(int c=0;c<3;c++)
     { 
       pTVerts[vt[c]]=v[c];
     }     
     CVector3 vVector1, vVector2, vNormal;
     vVector1 = v[2] - v[1];
     vVector2 = v[0] - v[1];	
     vNormal  = Cross(vVector1, vVector2);
     pTempNormals[i] = vNormal;			
     vNormal  = Normalize(vNormal);
     pTNormals[i] = vNormal;
     
  }//end for i
 if(smooth)
  for (i = 0; i < numVertices; i++)
  {
	for (j = 0; j < numTriangles; j++)
	{
	    if (triangles[j].v[0] == i || 
            triangles[j].v[1] == i || 
            triangles[j].v[2] == i)
	    {
		    vSum = vSum + pTempNormals[j];
			shared++;
		}
	}//end for j   
	//pTVNormals[i] = DivideVectorByScaler(vSum, float(-shared));
	pTVNormals[i] = vSum;
	pTVNormals[i].unitize();
	vSum = vZero;
	shared = 0;	
   }//end for i  
 delete [] pTempNormals; 
}

void MDF_OBJ::render_frame(int cframe)
{
  int i, c, v[3], uv[3];
  float x,y,z;
  
  glDisable (GL_TEXTURE_2D);    
  glColor3ub(255, 255, 255);	 
  
  for(i=0;i<numTriangles;i++)
  {
     v[0] = (int)triangles[i].v[0];
     v[1] = (int)triangles[i].v[1];
     v[2] = (int)triangles[i].v[2];
     //glBegin(GL_LINE_LOOP);    
     for(int c=0;c<3;c++)
     {              
       x = ((float)frames[cframe].vertices[v[c]].v[0] * frames[cframe].scale.x) + frames[cframe].translate.x;
       z = ((float)frames[cframe].vertices[v[c]].v[1] * frames[cframe].scale.y) + frames[cframe].translate.y;
       y = ((float)frames[cframe].vertices[v[c]].v[2] * frames[cframe].scale.z) + frames[cframe].translate.z;
       glVertex3f( x,y,z);
       //glVertex3f( pTVerts[v[c]].x, pTVerts[v[c]].y, pTVerts[v[c]].z);
     }//end for c
     //glEnd ();
     _polycount++;
  }
  glEnd ();  
}


void MDF_OBJ::render_flat()
{
  int i, c, v[3], uv[3];
  int tuv;
  
  for(i=0;i<numTriangles;i++)
  {
     v[0] = (int)triangles[i].v[0];
     v[1] = (int)triangles[i].v[1];
     v[2] = (int)triangles[i].v[2];
     
     uv[0] = (int)triangles[i].uv[0];
     uv[1] = (int)triangles[i].uv[1];
     uv[2] = (int)triangles[i].uv[2];
     
     //glBegin(GL_TRIANGLES);
     glNormal3f(pTNormals[i].x,pTNormals[i].y, pTNormals[i].z);     
     for(int c=0;c<3;c++)
     {              
         tuv = uv[c]<numTexCoords?uv[c]:0;         
         if(texturecoord)
            glTexCoord2f(texturecoord[tuv].x, texturecoord[tuv].y); 
                           
         glVertex3f( pTVerts[v[c]].x, pTVerts[v[c]].y, pTVerts[v[c]].z);
     }//end for c
     //glEnd ();
     _polycount++;
  }
  glEnd ();  
}

void MDF_OBJ::draw_normasl()
{
  int i,v[3];
  CVector3 center;
  
  glDisable (GL_TEXTURE_2D);    
  glColor3ub(255, 255, 255);
  
  for(i=0;i<numTriangles;i++)
  {
     v[0] = (int)triangles[i].v[0];
     v[1] = (int)triangles[i].v[1];
     v[2] = (int)triangles[i].v[2];
     
     center.init(0.0, 0.0, 0.0);
     for(int c=0;c<3;c++)
     {
        center += pTVerts[v[c]];
     }     
     center /= 3;          
     pTNormals[i].draw(&center, 4.0f);
     _polycount++;
  } 
}

void MDF_OBJ::render_smooth()
{
  int i, c, v[3], uv[3];
  for(i=0;i<numTriangles;i++)
  {
     v[0] = (int)triangles[i].v[0];
     v[1] = (int)triangles[i].v[1];
     v[2] = (int)triangles[i].v[2];
     
     uv[0] = (int)triangles[i].uv[0];
     uv[1] = (int)triangles[i].uv[1];
     uv[2] = (int)triangles[i].uv[2];
          
     for(int c=0;c<3;c++)
     {              
         glTexCoord2f(texturecoord[uv[c]].x, texturecoord[uv[c]].y); 
         glNormal3f(pTVNormals[v[c]].x,pTVNormals[v[c]].y, pTVNormals[v[c]].z);                  
         glVertex3f(pTVerts[v[c]].x   , pTVerts[v[c]].y  , pTVerts[v[c]].z   );
     }//end for c
     _polycount++;
  }
  glEnd ();  
}

void MDF_OBJ::gl_render_flat()
{
  int i;
  CVector3 uv;  
  long*	command;
  int vertIndex;
  int type;
  int numVertex;
  
  command		= glCommand; 
  while((*command)!=0)
  {
     if(*command>0)	//This is a triangle strip
	 {
		numVertex= *command; 
		command++; 
		type= 0;
     }
     else			//This is a triangle fan
     {
		numVertex= - *command; 
		command++; 
		type= 1;
     }

	 if(numVertex<0) numVertex= -numVertex;
     //If the .md2 was optimized for use with triangle strips...
	if(type==0)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i=0; i<numVertex; i++)
		{            
		    uv.x=  *((float*)command); 
			command++;
			uv.y=  1.0 - *((float*)command); 
			command++;
			vertIndex= *command; 
			command++;
                           
            glTexCoord2f(uv.x, uv.y);                           
            glVertex3f( pTVerts[vertIndex].x, pTVerts[vertIndex].y, pTVerts[vertIndex].z);
	    }
		glEnd();
     }//If the .md2 was made for use with triangle fans...
     else
     {
		glBegin(GL_TRIANGLE_FAN);
		for(i=0; i<numVertex; i++)
		{                     
		    uv.x=  *((float*)command); 
			command++;
			uv.y=  1.0 - *((float*)command); 
			command++;
			vertIndex= *command; 
			command++;
                         
            glTexCoord2f(uv.x, uv.y);                           
            glVertex3f( pTVerts[vertIndex].x, pTVerts[vertIndex].y, pTVerts[vertIndex].z);
	    }//endf for
        glEnd();
      }//end else if 
  }//end while    
}

void MDF_OBJ::gl_render_smooth()
{
  int i;
  CVector3 uv;  
  long*	command;
  int vertIndex;
  int type;
  int numVertex;
  
  command		= glCommand;  
  while((*command)!=0)
  {
     if(*command>0)	//This is a triangle strip
	 {
		numVertex= *command; 
		command++; 
		type= 0;
     }
     else			//This is a triangle fan
     {
		numVertex= - *command; 
		command++; 
		type= 1;
     }
	 if(numVertex<0) numVertex= -numVertex;
     //If the .md2 was optimized for use with triangle strips...
	if(type==0)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i=0; i<numVertex; i++)
		{       
		    uv.x=  *((float*)command); 
			command++;
			uv.y=  1.0 - *((float*)command); 
			command++;
			vertIndex= *command; 
			command++;
                           
            glTexCoord2f(uv.x, uv.y);
            glNormal3f(pTVNormals[vertIndex].x,pTVNormals[vertIndex].y, pTVNormals[vertIndex].z);                           
            glVertex3f( pTVerts[vertIndex].x, pTVerts[vertIndex].y, pTVerts[vertIndex].z);
	    }
		glEnd();
     }//If the .md2 was made for use with triangle fans...
     else
     {
		glBegin(GL_TRIANGLE_FAN);
		for(i=0; i<numVertex; i++)
		{
		    uv.x=  *((float*)command); 
			command++;
			uv.y=  1.0 - *((float*)command); 
			command++;
			vertIndex= *command; 
			command++;
                         
            glTexCoord2f(uv.x, uv.y); 
            glNormal3f(pTVNormals[vertIndex].x,pTVNormals[vertIndex].y, pTVNormals[vertIndex].z);                           
            glVertex3f( pTVerts[vertIndex].x, pTVerts[vertIndex].y, pTVerts[vertIndex].z);
	    }//endf for
        glEnd();
      }//end else if 
  }//end while    
}

void MDF_OBJ::scale(float _scale)
{
  if(frames)
  for(int i=0;i<numFrames;i++)
  {
     frames[i].scale *= _scale;
     frames[i].translate *= _scale;
  }
  calculate_bbox();
} 
///////////////////////////////////////////////////////////////////////////
//                           STANIMATION
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//                           STANIMATION
///////////////////////////////////////////////////////////////////////////

