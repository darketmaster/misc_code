#include "md2.h"
#include "log.h"

//MD2MODEL::MD2MODEL(char filename)
MD2MODEL::MD2MODEL(char *filename)
{
   load(filename);
}

//MD2MODEL~MD2MODEL()
MD2MODEL::~MD2MODEL()
{
   destroy();
}

//void MD2MODEL::load(char filename)   
void MD2MODEL::load(char *filename)
{
 FILE *fp;
 int i;
 
 fp=fopen(filename, "rb");
 if(fp)
 {
   
   
   fread(&magic           , sizeof(int), 1, fp);//magic            = pack_igetl(fp);   
   fread(&version         , sizeof(int), 1, fp);//version          = pack_igetl(fp);
   fread(&skinWidth       , sizeof(int), 1, fp);//skinWidth        = pack_igetl(fp);
   fread(&skinHeight      , sizeof(int), 1, fp);//skinHeight       = pack_igetl(fp);
   fread(&frameSize       , sizeof(int), 1, fp);//frameSize        = pack_igetl(fp);
   fread(&numSkins        , sizeof(int), 1, fp);//numSkins         = pack_igetl(fp);
   fread(&numVertices     , sizeof(int), 1, fp);//numVertices      = pack_igetl(fp);
   fread(&numTexCoords    , sizeof(int), 1, fp);//numTexCoords     = pack_igetl(fp);
   fread(&numTriangles    , sizeof(int), 1, fp);//numTriangles     = pack_igetl(fp);  
   fread(&numGlCommands   , sizeof(int), 1, fp);//numGlCommands    = pack_igetl(fp);
   fread(&numFrames       , sizeof(int), 1, fp);//numFrames        = pack_igetl(fp);
   fread(&offsetSkins     , sizeof(int), 1, fp);//offsetSkins      = pack_igetl(fp);    
   fread(&offsetTexCoords , sizeof(int), 1, fp);//offsetTexCoords  = pack_igetl(fp);     
   fread(&offsetTriangles , sizeof(int), 1, fp);//offsetTriangles  = pack_igetl(fp);    
   fread(&offsetFrames    , sizeof(int), 1, fp);//offsetFrames     = pack_igetl(fp);    
   fread(&offsetGlCommands, sizeof(int), 1, fp);//offsetGlCommands = pack_igetl(fp);     
   fread(&offsetEnd       , sizeof(int), 1, fp);//offsetEnd        = pack_igetl(fp);
   
   //TEXTURES COORD
   ////////////////////
   fseek(fp, offsetTexCoords, SEEK_SET);
   
   texturecoord = (textureCoordinate_t*)new textureCoordinate_t[numTexCoords];
   
   //size_t fread(void *buffer, size_t size, size_t number, FILE *file);
   fread(texturecoord, sizeof(textureCoordinate_t), numTexCoords , fp);
   
   //TRIANGLES
   ////////////////////
   fseek(fp, offsetTriangles, SEEK_SET);
   triangles = (triangle_t*)new triangle_t[numTriangles];
   
   fread(triangles, sizeof(triangle_t), numTriangles , fp);
   
   //FRAMES
   ////////////////////
   fseek(fp, offsetFrames, SEEK_SET);
   frames = (frame_t*)new frame_t[numFrames];
   
   for(i=0;i<numFrames;i++)
   {
       fread(frames[i].scale,  sizeof(float), 3,fp);
       fread(frames[i].translate , sizeof(float), 3, fp);
       fread(frames[i].name, sizeof(char), 16, fp);
       frames[i].vertices = new triangleVertex_t[numVertices];       
       fread(frames[i].vertices, sizeof(triangleVertex_t), numVertices, fp);
       
   }
   ///////////////////   
   fclose(fp);
 }//end if fp
}

//void MD2MODEL::destroy()
void MD2MODEL::destroy()
{
 int i;
  if(frames)
  {
     for(i=0;i<numFrames;i++) 
        if(frames[i].vertices)
           delete [] frames[i].vertices;
           
     delete(frames);
  }
  if(triangles) delete [] triangles;
  if(texturecoord) delete [] texturecoord;
}

//void MD2MODEL::debug()   
void MD2MODEL::debug()
{
 int i,j;
   add_separator_in_log();
   add_in_log("md2\n");
   add_in_log("MAGIC : %d\n",magic);
   add_in_log("version : %d\n",version);
   add_in_log("width : %d\n",skinWidth);
   add_in_log("height : %d\n",skinHeight);
   add_in_log("framesize : %d\n",frameSize);
   add_in_log("numskin : %d\n",numSkins);
   add_in_log("numvertices : %d\n",numVertices);
   add_in_log("numTexCoords : %d\n",numTexCoords);     
   add_in_log("numTriangles : %d\n",numTriangles);    
   add_in_log("numGlCommands : %d\n",numGlCommands);
   add_in_log("numFrames : %d\n",numFrames);
   add_in_log("offsetSkins : %d\n",offsetSkins);     
   add_in_log("offsetTexCoords : %d\n",offsetTexCoords);     
   add_in_log("offsetTriangles : %d\n",offsetTriangles);     
   add_in_log("offsetFrames : %d\n",offsetFrames); 
   add_in_log("offsetGlCommands : %d\n",offsetGlCommands); 
   add_in_log("offsetEnd : %d\n",offsetEnd); 
   
   add_in_log("texturecoord : %X\n",texturecoord);
   if(texturecoord)
      for(i=0;i<numTexCoords;i++)
      {
       add_in_log("[%04d] s: %d t: %d\n",i,texturecoord[i].s,texturecoord[i].t);
      }
   
   add_in_log("triangles : %X\n",triangles);
   if(triangles)
      for(i=0;i<numTriangles;i++)
      {
       add_in_log("[%04d] (%d, %d, %d) (%d, %d, %d)\n",i,
                   triangles[i].vertexIndices[0],triangles[i].vertexIndices[1],triangles[i].vertexIndices[2],
                   triangles[i].textureIndices[0],triangles[i].textureIndices[1],triangles[i].textureIndices[2]);
      }
   
   add_in_log("frames : %X\n",frames);
   if(frames)
      for(i=0;i<numFrames;i++)
      {
       add_in_log("[%04d]  %s, s(%f, %f, %f) t(%f, %f, %f)\n",i,
                   frames[i].name,
                   frames[i].scale[0],frames[i].scale[1],frames[i].scale[2],
                   frames[i].translate[0],frames[i].translate[1],frames[i].translate[2]);
      }  
         
   add_separator_in_log();

}
