// Texture.h: interface for the CMaterial class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <stdio.h>
//#include <ijl.h>
//#pragma comment (lib, "ijl15.lib")
#include <GL\gl.h>
#include <GL\glu.h>
//#include <GL\glh_genext.h>
//#include <gl\glaux.h>		// Header File For The Glaux Library
#include<gl\glext.h>

#define SAFEDEL(x) if(x !=NULL) {delete[] x; x = NULL;}
#pragma warning( disable : 4018 )

enum MAT_RENDER
{
	MAT,
	TEX,
	MAT_TEX,
	TEX_COLOR,
};
struct RGBA
{
	RGBA(float r=0, float g=0, float b=0, float a=0) : r(r), g(g), b(b),a(a) { }
	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		GLfloat m_rgba[4];
	};
};

#define MAX_EMBOSS (float)0.01f						// Maximum Emboss-Translate. Increase To Get Higher Immersion
#define __ARB_ENABLE true							// Used To Disable ARB Extensions Entirely
// #define EXT_INFO								// Uncomment To See Your Extensions At Start-Up?
#define MAX_EXTENSION_SPACE 10240						// Characters For Extension-Strings
#define MAX_EXTENSION_LENGTH 256						// Maximum Characters In One Extension-String

#if !defined(AFX_TEXTURE_H__F46069B2_AE7A_11D5_8A89_444553540000__INCLUDED_)
#define AFX_TEXTURE_H__F46069B2_AE7A_11D5_8A89_444553540000__INCLUDED_

#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's a ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)

#define BITMAP_ID 0x4D42

struct tMaterial
{
	unsigned int	m_iBpp;					// Image color depth in bits per pixel
	unsigned int	m_iWidth;				// Image width
	unsigned int	m_iHeight;				// Image height
	unsigned int	m_id;					// OpenGL Texture ID
	
	int				m_MaskTexID;		// Masking texture ID
	
	GLubyte			*m_cData;				// Pixel data
	char			m_strFilename[255];		// File name
	bool			m_bIsTex;				// Is it a texture? (Delete m_cData after loading and binding)
	unsigned int	m_iError;
	RGBA			m_Ambient, 
					m_Diffuse, 
					m_Emissive, 
					m_Specular;

	RGBA			m_Alpha;
	float m_fShininess;

	tMaterial() 
	{
		m_iBpp = 0;
		m_iWidth = 0;
		m_iHeight = 0;
		m_id = -1;
		m_cData = NULL;
		m_MaskTexID = -1;
		
		for (int i=0; i<4; i++)
		{
			m_Ambient.m_rgba[i] = 1.0;
			m_Diffuse.m_rgba[i] = 1.0;
			m_Emissive.m_rgba[i] = 1.0;
			m_Specular.m_rgba[i] = 1.0;
			m_Alpha.m_rgba[i] = -1.0;
		}
		m_fShininess = 0;
		strcpy(m_strFilename, "");
		m_iError = 0;
	}

	~tMaterial()
	{
		if(m_id > 0)
		{
			//glDeleteTextures(1, &m_id);
			m_id = 0;
		}
		if(m_cData !=NULL)
			SAFEDEL(m_cData);
	}
};

#define FILE_NOT_FOUND	WM_USER + 256
#define ERROR_ALLOC		WM_USER + 257
#define INVALID_TEXTURE_DIMENSIONS WM_USER + 258
#define CANNOT_LOAD		WM_USER + 259
#define INVALID_PIXEL_FORMAT WM_USER + 260
#define INVALID_FILE_FORMAT_SPEC WM_USER + 261

#endif // !defined(AFX_TEXTURE_H__F46069B2_AE7A_11D5_8A89_444553540000__INCLUDED_)

bool BitBlt(			tMaterial src,	tMaterial dst,
						GLshort src_xstart, GLshort src_ystart,
						GLshort src_xend,	GLshort src_yend,
						GLshort dst_xstart, GLshort dst_ystart,
						GLshort blend, GLshort alpha	);

bool isInString (char *string, char *search);
bool CheckSize (unsigned int Size);
GLubyte *ConvertPCX (GLubyte *dt, GLubyte *Palette, unsigned int Width, unsigned int Height);
	
		
// All works
bool LoadBMP(char* filename, tMaterial *mat, bool asTexture);
//bool LoadJPG(char* filename, tMaterial *mat, bool asTexture);
bool LoadTGA(char* filename, tMaterial *mat, bool asTexture);
bool LoadPCX(char* filename, tMaterial *mat, bool asTexture);
bool LoadRGB(char* filename, tMaterial *mat, bool asTexture);
bool LoadRAW(char* filename, int iSize, tMaterial *mat, bool asTexture);	// Not checked!

void CreateAlpha(tMaterial *mat, RGBA color);
void CreateGLTexture(tMaterial *mat);

bool LoadTexture(char *filename, tMaterial *mat, bool asTexture);
void ApplyMaterial(tMaterial *mat, MAT_RENDER rendermode = MAT_TEX, float *color = NULL);

bool SaveMaterial(tMaterial *mat, char *filename, FILE *f);
bool LoadMaterial(tMaterial *mat, char *filename, FILE *f);

bool SaveScreenshot(RECT Region, char *filename);
