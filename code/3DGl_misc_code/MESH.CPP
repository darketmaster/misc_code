#include <stdlib.h>
#include "MESH.H" 
#include "filelog.h" 
#include <stdlib.h>
#include <string.h>


#define get_new_line(file) if(!fgets (szLine, 256, (file))){ return 0; }

Mesh::Mesh()
{
   clear();
}

Mesh::~Mesh()
{
   destroy();
}

void Mesh::clear()
{
    material_index = -1;
    memset(Name, 0, MESH_MAX_NAME*sizeof(char));
    num_vertices = 0;
	vertices = NULL;
	num_triangles = 0;
	triangles = NULL;
	num_normals = 0;
	normals = NULL;
}    

void Mesh::destroy()
{
    if(vertices != NULL)
		delete[] vertices;	
	if(triangles != NULL)
		delete[] triangles;
	if(normals != NULL)
		delete[] normals;
	clear();
}

int  Mesh::save_txt_chunk(FILE *f)
{
    int j;
    
    fprintf(f, "\"%s\" %d\n", Name, material_index); 
    
    fprintf(f, "%d\n", num_vertices);    
    for (j = 0; j < num_vertices; j++)
    {
         fprintf (f, "%f %f %f %f %f %d\n",vertices[j].x, vertices[j].y, vertices[j].z,
                    vertices[j].u, (1.0f - vertices[j].v),&vertices[j].c);
    }
    
    fprintf(f, "%d\n", num_normals);    
    for (j = 0; j < num_normals; j++)
    {
         fprintf (f, "%f %f %f\n", normals[j].x,normals[j].y, normals[j].z);
    }
    
    fprintf(f, "%d\n", num_triangles);    
    for (j = 0; j < num_triangles; j++)
    {
         fprintf (f, "%d %d %d %d %d %d\n",
                     triangles[j].v[0], triangles[j].v[1], triangles[j].v[2], 
                     triangles[j].n[0], triangles[j].n[1], triangles[j].n[2]);
    }
}
    
int  Mesh::load_txt_chunk(FILE *f)
{
    char szLine[256];
    int j;
    
    get_new_line(f);
    if (sscanf (szLine, "\"%[^\"]\" %d %d",Name, &material_index) != 2)
        return 0;

    get_new_line(f);
    if (sscanf (szLine, "%d", &num_vertices) != 1)return 0;
			
   	vertices = new V3D_f[num_vertices];
    for (j = 0; j < num_vertices; j++)
    {
         get_new_line(f);
         if (sscanf (szLine, "%f %f %f %f %f %d",
                    &vertices[j].x, &vertices[j].y, &vertices[j].z,
                    &vertices[j].u, &vertices[j].v,
                    &vertices[j].c) != 6)
 					return 0;
		 vertices[j].v = 1.0f - vertices[j].v;
    }

    get_new_line(f);
    if (sscanf (szLine, "%d", &num_normals) != 1)return 0;

	normals = new CVector3[num_normals];
    for (j = 0; j < num_normals; j++)
    {
        get_new_line(f);
        if (sscanf (szLine, "%f %f %f",&normals[j].x, &normals[j].y, &normals[j].z) != 3)
            return 0;
    }
 
    get_new_line(f);
    if (sscanf (szLine, "%d", &num_triangles) != 1)	return 0;

	triangles = new _TRI[num_triangles];
    for (j = 0; j < num_triangles; j++)
    {
         get_new_line(f);
         if (sscanf (szLine, "%d %d %d %d %d %d",
                    &triangles[j].v[0], &triangles[j].v[1], &triangles[j].v[2],
                    &triangles[j].n[0], &triangles[j].n[1], &triangles[j].n[2]) != 6)
             return 0;
	}
}
    
int  Mesh::save_bin_chunk(FILE *f)
{
    if(!f) return 0; 
    
    fwrite(Name, sizeof(char), MESH_MAX_NAME, f); 
    fwrite(&material_index, sizeof(int), 1, f);
    
    fwrite(&num_vertices, sizeof(int), 1, f);
    fwrite(vertices, sizeof(V3D_f), num_vertices, f);
    
    fwrite(&num_normals, sizeof(int), 1, f);
    fwrite(normals, sizeof(CVector3), num_normals, f);
    
    fwrite(&num_triangles, sizeof(int), 1, f);
    fwrite(triangles, sizeof(_TRI), num_triangles, f);
    
    return 1;
}
    
int  Mesh::load_bin_chunk(FILE *f)
{
    if(!f) return 0; 
    
    fread(Name, sizeof(char), MESH_MAX_NAME, f); 
    fread(&material_index, sizeof(int), 1, f);
    
    fread(&num_vertices, sizeof(int), 1, f);
    vertices = new V3D_f[num_vertices];
    if(!vertices) return 0;
    fread(vertices, sizeof(V3D_f), num_vertices, f);
    
    fread(&num_normals, sizeof(int), 1, f);
    normals = new CVector3[num_normals];
    if(!normals) return 0;
    fread(normals, sizeof(CVector3), num_normals, f);
    
    fread(&num_triangles, sizeof(int), 1, f);
    triangles = new _TRI[num_triangles];
    if(!triangles) return 0;
    fread(triangles, sizeof(_TRI), num_triangles, f);
    
    return 1;
}
    
int  Mesh::save_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wt");
    if(f)
    {
        signature_save_file_t(f,MESH3DHEADER);
        save_txt_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;    
}
    
int  Mesh::load_txt(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rt");
    if(f)
    {
        if(!signature_load_file_t(f,MESH3DHEADER))
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
    
int  Mesh::save_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wb");
    if(f)
    {
        signature_save_file_b(f, strlen(MESH3DHEADER), MESH3DHEADER);
        save_bin_chunk(f);
        fclose(f);
    }
    else return 0;
    
    return 1;    
}
    
int  Mesh::load_bin(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rb");
    if(f)
    {
        if(!signature_load_file_b(f, strlen(MESH3DHEADER), MESH3DHEADER))
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

int Mesh::loadFromMs3dAsciiSegment( FILE *f )
{
    char szLine[256];
    int nFlags, nIndex, j;
    
    get_new_line(f);
    if (sscanf (szLine, "\"%[^\"]\" %d %d",Name, &nFlags, &material_index) != 3)
        return 0;
    
    // vertices
    get_new_line(f);
    if (sscanf (szLine, "%d", &num_vertices) != 1)return 0;
			
   	vertices = new V3D_f[num_vertices];

    for (j = 0; j < num_vertices; j++)
    {
         get_new_line(f);
         if (sscanf (szLine, "%d %f %f %f %f %f %d",
                    &nFlags,
                    &vertices[j].x, &vertices[j].y, &vertices[j].z,
                    &vertices[j].u, &vertices[j].v,
                    &vertices[j].c) != 7)
 					return 0;
		 // adjust the y direction of the texture coordinate
		 vertices[j].v = 1.0f - vertices[j].v;
    }

    // normals
    get_new_line(f);
    if (sscanf (szLine, "%d", &num_normals) != 1)return 0;

	normals = new CVector3[num_normals];

    for (j = 0; j < num_normals; j++)
    {
        get_new_line(f);
        if (sscanf (szLine, "%f %f %f",&normals[j].x, &normals[j].y, &normals[j].z) != 3)
            return 0;
    }
    
    // triangles
    get_new_line(f);
    if (sscanf (szLine, "%d", &num_triangles) != 1)	return 0;

	triangles = new _TRI[num_triangles];
    for (j = 0; j < num_triangles; j++)
    {
         get_new_line(f);
         if (sscanf (szLine, "%d %d %d %d %d %d %d %d",
                    &nFlags,
                    &triangles[j].v[0], &triangles[j].v[1], &triangles[j].v[2],
                    &triangles[j].n[0], &triangles[j].n[1], &triangles[j].n[2],
                    &nIndex ) != 8)
             return 0;
	}

	return 1;
}

    

