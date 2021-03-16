#include <string.h>
#include <stdio.h>
#include <windows.h>
//#include "types.h"

#ifndef ASSERT
#define ASSERT(x)		if(!(x)){ printf("ERROR %d\n", __LINE__); exit(1);} 
#endif

#define MAX_POLY_SIDES	20
#define MAX_VERTICES	20000		// increase if you need to
#define MAX_POLYGONS	20000		// increase if you need to


typedef struct V3D
{
float x,y,z;
float u,v;
int c;
}V3D;

typedef struct POLY              
{
  	int		count;
   	int 	v[ MAX_POLY_SIDES];		// offsets into vertices[]
   	int		node_id;				// index into nodes[]
   	int		plane_id;				// index into planes[]
   	int		added;					// added to tree yet?  (not really used at the moment)
} POLY;

typedef struct VEC
{
	double x,y,z;
} VEC;

typedef struct CPOLY
{
   int 	v[ MAX_POLY_SIDES]; 
}CPOLY;    

V3D		vertices[ MAX_VERTICES ];
V3D		uvs[ MAX_VERTICES ];
POLY	polygons[ MAX_POLYGONS ];
VEC		normals[ MAX_POLYGONS ];

/////////////////////////new
VEC		vnormals[ MAX_VERTICES ];
VEC     cvertices[ MAX_VERTICES ];
CPOLY   cpolygons[ MAX_POLYGONS ];
CPOLY   vpnormals[ MAX_POLYGONS ];

static int 	num_vertices = 0;
static int 	num_polygons = 0;

/////////////////////////new
static int 	num_vnormals = 0;
static int 	num_cvertices = 0;


/*
.VP file description
====================

V: number of vertices
x y z u v 
...
...
P: number of polygons
vertexcount v[0] v[1] .... v[vertexcount] Nx Ny Nz
...
...
////////////////new
V: number of normals vertices
x y z
V: number of color vertices
fr fg rb
CP: number of color polygons
v[0] v[1] v[3]
*/
int save_vp( char *filename )
{
	FILE 	*fp;
	int		i,j;
	float	fx, fy, fz, fu, fv;
	int		c;
	int		vn;
	int cv[6];
	
	fp = fopen(filename, "wt");
	if( fp == NULL )
	{
		//printf("Can't open output file %s\n", filename );
        return 1;
	}
	fprintf(fp, "VERT: %d\n", num_vertices);
    fprintf(fp, "POLY: %d\n", num_polygons);
	fprintf(fp, "%d\n", num_vertices);
	for(i = 0; i < num_vertices; i++)
	{
		fx = vertices[i].x;
		fy = vertices[i].y;
		fz = vertices[i].z;
		fu = vertices[i].u;
		fv = vertices[i].v;

		fprintf(fp, "%f %f %f %f %f\n", fx, fy, fz, fu, fv);
	}

	fprintf(fp, "%d\n", num_polygons);
	for(i = 0; i < num_polygons; i++)
	{
		c = polygons[i].count;
		ASSERT(c >= 3);
		ASSERT( c < MAX_POLY_SIDES );
		fprintf(fp, "%d", c);

		for(j = 0; j < c; j++)
		{
			ASSERT(  polygons[i].v[j] < num_vertices );
			fprintf(fp, " %d", polygons[i].v[j]);
		}

		fprintf(fp, " %f %f %f\n", normals[i].x, normals[i].y, normals[i].z );
	}
	
	////////////////////////////new
	fprintf(fp, "%d\n", num_vnormals);
	for(i = 0; i < num_vnormals; i++)
	{
		fx = vnormals[i].x;
		fy = vnormals[i].y;
		fz = vnormals[i].z;

		fprintf(fp, "%f %f %f\n", fx, fy, fz);
	}
	
	fprintf(fp, "%d\n", num_cvertices);
	for(i = 0; i < num_cvertices; i++)
	{
		fx = cvertices[i].x;
		fy = cvertices[i].y;
		fz = cvertices[i].z;

		fprintf(fp, "%f %f %f\n", fx, fy, fz);
	}
	
	if(num_cvertices>0)
	{
	   fprintf(fp, "%d\n", num_polygons);
	   for(i = 0; i < num_polygons; i++)
	   {
	    	cv[0] = cpolygons[i].v[0];
	    	cv[1] = cpolygons[i].v[1];
	    	cv[2] = cpolygons[i].v[2];
	    	cv[3] = vpnormals[i].v[0];
	    	cv[4] = vpnormals[i].v[1];
	    	cv[5] = vpnormals[i].v[2]; 
      		fprintf(fp, "%d %d %d %d %d %d\n", cv[0], cv[1], cv[2], cv[3], cv[4], cv[5]);
	   }
    }    	
	
	return 0;
}

void vertex_uv( FILE *flog, int poly, int vn, int uv )
{
	int k;
	int	i;
	int	found;

	ASSERT( poly <= num_polygons );
	ASSERT( vn >= 0);
	ASSERT( vn <= 2);

	k = polygons[poly].v[vn];
	fprintf(flog,"vertex %d c=%d num_vertices=%d\n", k, vertices[k].c, num_vertices );
	if( vertices[k].c != -1 && (vertices[k].u != uvs[uv].u || vertices[k].v != uvs[uv].v)  )
	{
		found = 0;
		for(i = 0; i <= num_vertices; i++)
		{
			if( vertices[i].x == vertices[k].x &&
				vertices[i].y == vertices[k].y &&
				vertices[i].z == vertices[k].z &&
				vertices[i].u == uvs[uv].u &&
			    vertices[i].v == uvs[uv].v )
			{
				k = i;
				found = 1;
				break;
			}
		}

		if(!found)
		{
			num_vertices++;
			ASSERT( num_vertices < MAX_VERTICES );
			vertices[num_vertices].x = vertices[k].x;
			vertices[num_vertices].y = vertices[k].y;
			vertices[num_vertices].z = vertices[k].z;
			k = num_vertices;
		}
	}
	vertices[k].c = 0;
	vertices[k].u = uvs[uv].u;
	vertices[k].v = uvs[uv].v;
	polygons[poly].v[vn] = k;
}

// load model in 3D studio ASCII scene output format
//
int load_ase(HWND hwnd, char *filename,char *path)
{
	FILE	*fp;
	FILE	*flog;
	char	line[80];
	char	*p;
//	int		n = 0;
	int		i,j,nv;
	int		va, vb, vc;
	char	ca, cb, cc;
	double 	fa, fb, fc;
	char	name[80];
//	int		num_uv = 0;

   char name_file[80];


	flog = fopen("ase_log.txt", "wt");
	if(flog == NULL )
	{
       return 1;
	}


	fp = fopen(filename, "rt");
	if(fp == NULL )
	{
       fclose(flog);
       return 2;
	}

	name[0] = '\0';
	num_polygons = 0;
	num_vertices = 0;


	p = fgets(line, sizeof(line), fp);
	while(p != NULL)
	{
		while(*p == '\t')
			p++;

		if(strncmp(p, "*NODE_NAME", strlen("*NODE_NAME")) == 0 )
		{
			//printf(p+11);
			// *NODE_NAME "Box01"
			sscanf(p+11, "\"%s", name );
			name[strlen(name)-1] = '\0';		// remove last char (")
			fprintf(flog, "name [%s]\n", name );
		}
		else if(strncmp(p, "*MESH_VERTEX_LIST", strlen("*MESH_VERTEX_LIST")) == 0 )
		{
			fprintf(flog, p);
		}
		else if(strncmp(p, "*MESH_VERTEX ",  strlen("*MESH_VERTEX ")) == 0 )
		{
			//printf(p);
			//	*MESH_VERTEX    0	-38.5542	-35.1807	0.0000
			sscanf(p+12, "%d %lf %lf %lf", &i, &fa, &fb, &fc );
			fprintf(flog,"vertex %d [%f %f %f]\n", i, fa, fb, fc );
			ASSERT( i < MAX_VERTICES );


			vertices[i].x = fa;
			vertices[i].y = -fc;		// note: swap z & y and negate y
			vertices[i].z = fb;
			vertices[i].u = 0;
			vertices[i].v = 0;
			vertices[i].c = -1;

			if(i > num_vertices)
				num_vertices = i;
		}
		////////////////////////new
		else if(strncmp(p, "*MESH_CVERTLIST", strlen("*MESH_CVERTLIST ")) == 0 )
		{
			fprintf(flog, p);
		}
		else if(strncmp(p, "*MESH_VERTCOL ",  strlen("*MESH_VERTCOL ")) == 0 )
		{
			//printf(p);
			//   *MESH_VERTCOL 0	1.0000	1.0000	1.0000
			sscanf(p+14, "%d %lf %lf %lf", &i, &fa, &fb, &fc );
			fprintf(flog,"vertexcolor %d [%f %f %f]\n", i, fa, fb, fc );
			ASSERT( i < MAX_VERTICES );

			cvertices[i].x = fa;
			cvertices[i].y = fb;
			cvertices[i].z = fc;

			if(i > num_cvertices)
				num_cvertices = i;
		}
		//////////////////////////////////////////////////////////////////////
		else if(strncmp(p, "*MESH_FACE_LIST", strlen("*MESH_FACE_LIST")) == 0 )
		{
			//printf(p);
		}
		else if(strncmp(p, "*MESH_FACE ",  strlen("*MESH_FACE ")) == 0 )
		{
			//printf(p);

			//MESH_FACE 0:    A:0 B:5 C:6 AB:1 BC:1 CA:1 ...

			sscanf(p+10, "%d: %c:%d %c:%d %c:%d", &i, &ca, &va, &cb, &vb, &cc, &vc );
			fprintf(flog,"face %d [%d %d %d]\n", i, va, vb, vc );
			ASSERT( i < MAX_POLYGONS );

			polygons[i].count = 3;
			polygons[i].v[0] = va;
			polygons[i].v[1] = vb;
			polygons[i].v[2] = vc;
			ASSERT(va <= num_vertices );
			ASSERT(vb <= num_vertices );
			ASSERT(vc <= num_vertices );

			if(i > num_polygons)
				num_polygons = i;
		}
		//////////////////////new////////////////////////////////////////////////
		else if(strncmp(p, "*MESH_CFACELIST", strlen("*MESH_CFACELIST")) == 0 )
		{
			//printf(p);
		}
		else if(strncmp(p, "*MESH_CFACE ",  strlen("*MESH_CFACE ")) == 0 )
		{
			//printf(p);
			// *MESH_CFACE 0	0	4	6
			sscanf(p+12, "%d %d %d %d", &i, &va, &vb, &vc );
			fprintf(flog,"Cface %d [%d %d %d]\n", i, va, vb, vc );
			ASSERT( i < MAX_POLYGONS );

			cpolygons[i].v[0] = va;
			cpolygons[i].v[1] = vb;
			cpolygons[i].v[2] = vc;
			ASSERT(va <= num_cvertices );
			ASSERT(vb <= num_cvertices );
			ASSERT(vc <= num_cvertices );
		}
/////////////////////////////////////////////////////////////////////////////////////
		else if(strncmp(p, "*MESH_FACENORMAL", strlen("*MESH_FACENORMAL")) == 0 )
		{
			// *MESH_FACENORMAL 0	0.0000	-0.9371	0.3491
			sscanf(p+17, "%d %lf %lf %lf", &i, &fa, &fb, &fc );
			fprintf(flog,"Normal(%d) (%f, %f, %f)\n", i, fa, fb, fc );
			ASSERT( i < MAX_POLYGONS );

			normals[i].x = fa;
			normals[i].y = -fc;
			normals[i].z = fb;
			
     		for (j=0;j<3;j++)
     		{
    		  p = fgets(line, sizeof(line), fp);
     		  while(*p == '\t') p++;
              if(strncmp(p, "*MESH_VERTEXNORMAL", strlen("*MESH_VERTEXNORMAL")) == 0 )
		      {
			   //  *MESH_VERTEXNORMAL 11	-0.1015	0.5680	0.8167
			     sscanf(p+19, "%d %lf %lf %lf", &nv, &fa, &fb, &fc );
			     fprintf(flog,"Vertexnormal(%d) (%f, %f, %f)\n", nv, fa, fb, fc );
			     ASSERT( nv < MAX_VERTICES );
			     vnormals[nv].x = fa;
			     vnormals[nv].y = -fc;
		         vnormals[nv].z = fb;
		         vpnormals[i].v[j] = nv;
			     if(nv > num_vnormals)	num_vnormals = nv;
		       }
           }//end for      			
		}		

		else if(strncmp(p, "*MESH_TVERTLIST", strlen("*MESH_TVERTLIST")) == 0 )
		{
			fprintf(flog,p);
		}
		else if(strncmp(p, "*MESH_TVERT",  strlen("*MESH_TVERT")) == 0 )
		{
			//printf(p);
			//	*MESH_TVERT 0	0.2681	0.1476	0.0000
			sscanf(p+12, "%d %lf %lf %lf", &i, &fa, &fb, &fc );
			fprintf(flog,"uv %d [%f %f]\n", i, fa, fb );
			ASSERT( i < MAX_VERTICES );

			uvs[i].u = fa;
			uvs[i].v = fb;
		}


		else if(strncmp(p, "*MESH_TFACELIST ", strlen("*MESH_TFACELIST ")) == 0 )
		{
			//printf(p);
		}

		else if(strncmp(p, "*MESH_TFACE",  strlen("*MESH_TFACE")) == 0 )
		{
			int	k;

			fprintf(flog, p);

			//MESH_TFACE 0	0	2	10

			sscanf(p+12, "%d %d %d %d", &i, &va, &vb, &vc );
			fprintf(flog,"face %d uv's: [%d %d %d]\n", i, va, vb, vc );
			ASSERT( i < MAX_POLYGONS );

			vertex_uv(flog,  i, 0, va );
			vertex_uv(flog,  i, 1, vb );
			vertex_uv(flog,  i, 2, vc );

		}

		if( line[0] == '}' && name[0] != '\0')
		{
			char fname[80];

			sprintf(fname, "%s.cvp", name);

			num_vertices++;
			num_polygons++;
			if(num_vnormals)num_vnormals++;
            if(num_cvertices)num_cvertices++;

//			printf("end of %s\n", name );
//			printf("vertices: %d polygons: %d\n", num_vertices, num_polygons);
//			printf("saving as %s\n", fname);
			fprintf(flog, "end of %s\n", name );
			fprintf(flog,"vertices: %d polygons: %d\n", num_vertices, num_polygons);
			fprintf(flog,"saving as %s\n", fname);

	// sprintf(name_file,"%s/%s",path,fname);
            sprintf(name_file,"%s",fname);
			if(save_vp(name_file)==1)
            {
               sprintf(name_file,"%s \n no file create",fname);
               MessageBox(hwnd, name_file, "Warning", MB_OK);
            }

			name[0] = '\0';
			num_polygons = 0;
			num_vertices = 0;
			num_vnormals = 0;
            num_cvertices = 0;
		}


		p = fgets(line, sizeof(line), fp);

	}


	fclose(fp);
	fclose(flog);
	return 0;
}

#include "ASE2VPRC.h"

static HINSTANCE hInstance;

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static char szFile[256];
    static char szDirName[256]=".\\";
    char szText[40];
    OPENFILENAME ofn;
    int id, bRet;

	switch(Message)
	{
		case WM_INITDIALOG:
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{

              case ID_CONVERT :

                    if (! *szDirName)
                        GetSystemDirectory(szDirName, sizeof(szDirName));

                    *szFile = '\0';
                    memset(&ofn, 0, sizeof(ofn));

                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.hInstance = hInstance;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrInitialDir = szDirName;

                    if (wParam == ID_CONVERT) {
                        ofn.lpstrFilter = "ASE File (*.ASE)\0*.ASE\0";
                        ofn.lpstrTitle = "Select ASE file to convert";
                        ofn.Flags = OFN_SHOWHELP | OFN_FILEMUSTEXIST;
                        bRet = GetOpenFileName(&ofn);
                    }

                    if (bRet) {
                        char *s;

                        switch(load_ase(hwnd,szFile,NULL))
                        {
                         case 0:
                              MessageBox(hwnd, "Convert complete.", "Warning", MB_OK);
                         break;
                         case 1:
                              MessageBox(hwnd, "Can't open log file.", "Warning", MB_OK);
                         break;
                         case 2:
                              MessageBox(hwnd, "Can't open Ase file", "Warning", MB_OK);
                         break;
                        }

                        // set standard Directory Path
                        strcpy(szDirName, szFile);
                        s = szDirName + strlen(szDirName);
                        while (s > szDirName && *--s != '\\')
                            ;
                        *s = '\0';

                    }
                    break;
			  }
		break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
		break;
		default:
			return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{

   hInstance =  hInst;

	return DialogBox(hInst, MAKEINTRESOURCE(ID_MAINDLG), NULL, DlgProc);
}
