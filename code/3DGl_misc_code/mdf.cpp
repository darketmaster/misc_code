#include "mdf.h"
#include "log.h"
/////////////////////////////////////////////////////////////////////////////
//                        MDFMODEL
/////////////////////////////////////////////////////////////////////////////


MDFMODEL::MDFMODEL()
{
   blank();
}

MDFMODEL::~MDFMODEL()
{
   destroy();
}

void MDFMODEL::blank()
{
   frameSize = 0; 
   numVertices = 0; 
   numTexCoords = 0; 
   numTriangles = 0; 
   numFrames = 0; 
   offsetTexCoords = 0; 
   offsetTriangles = 0; 
   offsetFrames = 0; 
   offsetEnd = 0; 
   
   numGlCommands = 0;
   offsetGlCommands = 0;
   
   glCommand = NULL;
   
   frames = NULL;
   triangles = NULL;
   texturecoord = NULL;
}

void MDFMODEL::load_md2(char *filename)
{
 FILE *fp;
 int i;
 int magic; 
 int version; 
 int skinWidth; 
 int skinHeight; 
 int numSkins; 
 int offsetSkins; 
 texCoord_t *temp_tex;
 float x,y; 
 
 fp=fopen(filename, "rb");
 if(fp)
 {
   fread(&magic           , sizeof(int), 1, fp);   //aux
   fread(&version         , sizeof(int), 1, fp);   //aux
   fread(&skinWidth       , sizeof(int), 1, fp);   //aux
   fread(&skinHeight      , sizeof(int), 1, fp);   //aux
   fread(&frameSize       , sizeof(int), 1, fp);   //1
   fread(&numSkins        , sizeof(int), 1, fp);   //aux
   fread(&numVertices     , sizeof(int), 1, fp);   //2
   fread(&numTexCoords    , sizeof(int), 1, fp);   //3
   fread(&numTriangles    , sizeof(int), 1, fp);   //4
   fread(&numGlCommands   , sizeof(int), 1, fp);   //5
   fread(&numFrames       , sizeof(int), 1, fp);   //6
   fread(&offsetSkins     , sizeof(int), 1, fp);   //aux 
   fread(&offsetTexCoords , sizeof(int), 1, fp);   //7
   fread(&offsetTriangles , sizeof(int), 1, fp);   //8
   fread(&offsetFrames    , sizeof(int), 1, fp);   //9
   fread(&offsetGlCommands, sizeof(int), 1, fp);   //10  
   fread(&offsetEnd       , sizeof(int), 1, fp);   //11  
    
   //TEXTURES COORD
   ////////////////////
   fseek(fp, offsetTexCoords, SEEK_SET);   
   temp_tex = (texCoord_t*)new texCoord_t[numTexCoords];
   fread(temp_tex, sizeof(texCoord_t), numTexCoords , fp); 
     
   texturecoord = (CVector2*)new CVector2[numTexCoords];
   
   if(skinWidth!=0) x = float(skinWidth);
   else x = 1;            
   if(skinHeight!=0) y = float(skinHeight);
   else y = 1; 
           
   for(int c=0; c<numTexCoords; c++)
   {
       texturecoord[c].x = float(temp_tex[c].s) / x;
       texturecoord[c].y = 1.0 - float(temp_tex[c].t) / y;
   }    
   delete [] temp_tex;   
   
   //TRIANGLES
   ////////////////////
   fseek(fp, offsetTriangles, SEEK_SET);
   triangles = (tri_mdf*)new tri_mdf[numTriangles];   
   fread(triangles, sizeof(tri_mdf), numTriangles , fp);
   
   //FRAMES
   ////////////////////
   fseek(fp, offsetFrames, SEEK_SET);
   frames = (mdf_frame_t*)new mdf_frame_t[numFrames];
   
   for(i=0;i<numFrames;i++)
   {
       triVertex_t *tv;
       char name[16];
       
       tv = (triVertex_t*)new triVertex_t[numVertices];
       frames[i].vertices = (mdf_tri_t*)new mdf_tri_t[numVertices];
       
       fread(&frames[i].scale     , sizeof(CVector3) ,         1, fp);
       fread(&frames[i].translate , sizeof(CVector3) ,         1, fp);       
       fread(name, sizeof(char), 16, fp);
       
       fread(tv, sizeof(triVertex_t), numVertices, fp);
        
       for(int c=0; c<numVertices; c++)
       {
           frames[i].vertices[c].v[0] = tv[c].vertex[0];
           frames[i].vertices[c].v[1] = tv[c].vertex[1];
           frames[i].vertices[c].v[2] = tv[c].vertex[2];
       }       
       delete [] tv;
             
   }
   
   glCommand = (long*)new long [numGlCommands*sizeof(long)];
   fseek(fp,  offsetGlCommands, SEEK_SET);
   fread(glCommand, sizeof(long), numGlCommands, fp);
   
   frameSize = (sizeof(CVector3)*2) + (sizeof(mdf_tri_t)*numVertices);
   
   offsetTexCoords = sizeof(int) * 12;    //12 for the signature number
   offsetTriangles = offsetTexCoords + sizeof(CVector2)*numTexCoords;    
   offsetFrames = offsetTriangles + sizeof(tri_mdf)*numTriangles;
   offsetGlCommands = offsetFrames + numFrames*frameSize;
   offsetEnd = offsetGlCommands + sizeof(long)*numGlCommands;   
         
   fclose(fp);
 }//end if fp
}

//void MDFMODEL::destroy()
void MDFMODEL::destroy()
{
 int i;
  if(frames)
  {
     for(i=0;i<numFrames;i++) 
        if(frames[i].vertices)
           delete [] frames[i].vertices;
           
     delete(frames);
  }
  
  if(glCommand) delete [] glCommand;
  
  if(triangles) delete [] triangles;
  if(texturecoord) delete [] texturecoord;  
  blank();
}

//void MDFMODEL::debug()   
void MDFMODEL::debug()
{
 int i,j;
   add_separator_in_log();
   add_in_log("mdf\n");
   add_in_log("framesize       : %d\n",frameSize);
   add_in_log("numvertices     : %d\n",numVertices);
   add_in_log("numTexCoords    : %d\n",numTexCoords);     
   add_in_log("numTriangles    : %d\n",numTriangles); 
   add_in_log("numGlCommands   : %d\n",numGlCommands);   
   add_in_log("numFrames       : %d\n",numFrames);
   add_in_log("offsetTexCoords : %d\n",offsetTexCoords);     
   add_in_log("offsetTriangles : %d\n",offsetTriangles);     
   add_in_log("offsetFrames    : %d\n",offsetFrames); 
   add_in_log("offsetGlCommands: %d\n",offsetGlCommands); 
   add_in_log("offsetEnd       : %d\n",offsetEnd); 
   
   add_in_log("texturecoord : %X\n",texturecoord);
   if(texturecoord)
      for(i=0;i<numTexCoords;i++)
      {
       add_in_log("[%04d] s: %f t: %f\n",i,texturecoord[i].x,texturecoord[i].y);
      }
   
   add_in_log("triangles : %X\n",triangles);
   if(triangles)
      for(i=0;i<numTriangles;i++)
      {
       add_in_log("[%04d] (%04d, %04d, %04d) (%04d, %04d, %04d)\n",i,
                   triangles[i].v[0],triangles[i].v[1],triangles[i].v[2],
                   triangles[i].uv[0],triangles[i].uv[1],triangles[i].uv[2]);
      }
   
   add_in_log("frames : %X\n",frames);
   if(frames)
      for(i=0;i<numFrames;i++)
      {
       add_in_log("[%04d]  s(%.4f, %.4f, %.4f) t(%.4f, %.4f, %.4f)\n",i,
                   frames[i].scale.x    , frames[i].scale.y   , frames[i].scale.z,
                   frames[i].translate.x,frames[i].translate.y,frames[i].translate.z);
      }  
         
   add_separator_in_log();
}



void MDFMODEL::save_mdf(char *filename)
{
 FILE *fp;
 int i;
 int sig = MDFSIG; 
 texCoord_t *temp_tex;
 float x,y; 
 
 fp=fopen(filename, "wb");
 if(fp)
 {    
   fwrite(&sig             , sizeof(int), 1, fp);   //aux
   fwrite(&frameSize       , sizeof(int), 1, fp);   //1
   fwrite(&numVertices     , sizeof(int), 1, fp);   //2
   fwrite(&numTexCoords    , sizeof(int), 1, fp);   //3
   fwrite(&numTriangles    , sizeof(int), 1, fp);   //4
   fwrite(&numGlCommands   , sizeof(int), 1, fp);   //5
   fwrite(&numFrames       , sizeof(int), 1, fp);   //6
   fwrite(&offsetTexCoords , sizeof(int), 1, fp);   //7
   fwrite(&offsetTriangles , sizeof(int), 1, fp);   //8
   fwrite(&offsetFrames    , sizeof(int), 1, fp);   //9
   fwrite(&offsetGlCommands, sizeof(int), 1, fp);   //10
   fwrite(&offsetEnd       , sizeof(int), 1, fp);   //11   
   //textures
   fwrite(texturecoord, sizeof(CVector2), numTexCoords , fp);    
   //TRIANGLES
   ////////////////////
   fwrite(triangles, sizeof(tri_mdf), numTriangles , fp);   
   //FRAMES
   ////////////////////  
   for(i=0;i<numFrames;i++)
   {       
       fwrite(&frames[i].scale     , sizeof(CVector3) ,         1, fp);
       fwrite(&frames[i].translate , sizeof(CVector3) ,         1, fp);       
       fwrite(frames[i].vertices, sizeof(mdf_tri_t), numVertices, fp);             
   } 
   
   fwrite(glCommand, sizeof(long), numGlCommands, fp);  
                           
   fclose(fp);
 }//end if fp
}

void MDFMODEL::load_mdf(char *filename)
{
 FILE *fp;
 int i;
 int sig; 

 fp=fopen(filename, "rb");
 if(fp)
 {
    
   fread(&sig             , sizeof(int), 1, fp);   //aux
   if(sig!=MDFSIG)  
   {
      fclose(fp);
      return;
   } 

   fread(&frameSize       , sizeof(int), 1, fp);   //1
   fread(&numVertices     , sizeof(int), 1, fp);   //2
   fread(&numTexCoords    , sizeof(int), 1, fp);   //3
   fread(&numTriangles    , sizeof(int), 1, fp);   //4
   fread(&numGlCommands   , sizeof(int), 1, fp);   //5
   fread(&numFrames       , sizeof(int), 1, fp);   //6
   fread(&offsetTexCoords , sizeof(int), 1, fp);   //7
   fread(&offsetTriangles , sizeof(int), 1, fp);   //8
   fread(&offsetFrames    , sizeof(int), 1, fp);   //9
   fread(&offsetGlCommands, sizeof(int), 1, fp);   //10
   fread(&offsetEnd       , sizeof(int), 1, fp);   //11 
       
   //TEXTURES COORD
   ////////////////////
   fseek(fp, offsetTexCoords, SEEK_SET);   
   texturecoord = (CVector2*)new CVector2[numTexCoords];
   fread(texturecoord, sizeof(CVector2), numTexCoords , fp);    
   
   //TRIANGLES
   ////////////////////
   fseek(fp, offsetTriangles, SEEK_SET);
   triangles = (tri_mdf*)new tri_mdf[numTriangles];   
   fread(triangles, sizeof(tri_mdf), numTriangles , fp);
   
   //FRAMES
   ////////////////////
   fseek(fp, offsetFrames, SEEK_SET);
   frames = (mdf_frame_t*)new mdf_frame_t[numFrames];
   
   for(i=0;i<numFrames;i++)
   {
       frames[i].vertices = (mdf_tri_t*)new mdf_tri_t[numVertices];       
       fread(&frames[i].scale     , sizeof(CVector3) ,         1, fp);
       fread(&frames[i].translate , sizeof(CVector3) ,         1, fp);       
       fread(frames[i].vertices   , sizeof(mdf_tri_t), numVertices, fp);            
   }
   
   fseek(fp,  offsetGlCommands, SEEK_SET);
   glCommand = (long*)new long [numGlCommands*sizeof(long)];   
   fread(glCommand, sizeof(long), numGlCommands, fp);
   
   fclose(fp);
 }//end if fp
}

void MDFMODEL::scale(float _scale)
{
  if(frames)
  for(int i=0;i<numFrames;i++)
  {
     frames[i].scale *= _scale;
     frames[i].translate *= _scale;
  }
} 


