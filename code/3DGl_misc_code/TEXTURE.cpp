// Texture.cpp: implementation of the CMaterial class.
//
//////////////////////////////////////////////////////////////////////
#include "Texture.h"

// RGBA.C SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "filelog.h"

void
bwtorgba(unsigned char *b,unsigned char *l,int n) {
    while(n--) {
	l[0] = *b;
	l[1] = *b;
	l[2] = *b;
	l[3] = 0xff;
	l += 4; b++;
    }
}

void
latorgba(unsigned char *b, unsigned char *a,unsigned char *l,int n) {
    while(n--) {
	l[0] = *b;
	l[1] = *b;
	l[2] = *b;
	l[3] = *a;
	l += 4; b++; a++;
    }
}

void
rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n) {
    while(n--) {
	l[0] = r[0];
	l[1] = g[0];
	l[2] = b[0];
	l[3] = 0xff;
	l += 4; r++; g++; b++;
    }
}

void
rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *a,unsigned char *l,int n) {
    while(n--) {
	l[0] = r[0];
	l[1] = g[0];
	l[2] = b[0];
	l[3] = a[0];
        l += 4; r++; g++; b++; a++;
    }
}

typedef struct _ImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short xsize, ysize, zsize;
    unsigned int min, max;
    unsigned int wasteBytes;
    char name[80];
    unsigned long colorMap;
    FILE *file;
    unsigned char *tmp, *tmpR, *tmpG, *tmpB;
    unsigned long rleEnd;
    unsigned int *rowStart;
    int *rowSize;
} ImageRec;

static void
ConvertShort(unsigned short *array, long length) {
    unsigned b1, b2;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	*array++ = (b1 << 8) | (b2);
    }
}

static void
ConvertLong(unsigned *array, long length) {
    unsigned b1, b2, b3, b4;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	b3 = *ptr++;
	b4 = *ptr++;
	*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
    }
}

static ImageRec *ImageOpen(const char *fileName)
{
    union {
	int testWord;
	char testByte[4];
    } endianTest;
    ImageRec *image;
    int swapFlag;
    int x;

    endianTest.testWord = 1;
    if (endianTest.testByte[0] == 1) {
	swapFlag = 1;
    } else {
	swapFlag = 0;
    }

    image = (ImageRec *)malloc(sizeof(ImageRec));
    if (image == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    if ((image->file = fopen(fileName, "rb")) == NULL) {
	perror(fileName);
	exit(1);
    }

    fread(image, 1, 12, image->file);

    if (swapFlag) {
	ConvertShort(&image->imagic, 6);
    }

    image->tmp = (unsigned char *)malloc(image->xsize*256);
    image->tmpR = (unsigned char *)malloc(image->xsize*256);
    image->tmpG = (unsigned char *)malloc(image->xsize*256);
    image->tmpB = (unsigned char *)malloc(image->xsize*256);
    if (image->tmp == NULL || image->tmpR == NULL || image->tmpG == NULL ||
	image->tmpB == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    if ((image->type & 0xFF00) == 0x0100) {
	x = image->ysize * image->zsize * sizeof(unsigned);
	image->rowStart = (unsigned *)malloc(x);
	image->rowSize = (int *)malloc(x);
	if (image->rowStart == NULL || image->rowSize == NULL) {
	    fprintf(stderr, "Out of memory!\n");
	    exit(1);
	}
	image->rleEnd = 512 + (2 * x);
	fseek(image->file, 512, SEEK_SET);
	fread(image->rowStart, 1, x, image->file);
	fread(image->rowSize, 1, x, image->file);
	if (swapFlag) {
	    ConvertLong(image->rowStart, x/(int)sizeof(unsigned));
	    ConvertLong((unsigned *)image->rowSize, x/(int)sizeof(int));
	}
    } else {
	image->rowStart = NULL;
	image->rowSize = NULL;
    }
    return image;
}

static void
ImageClose(ImageRec *image) {
    fclose(image->file);
    free(image->tmp);
    free(image->tmpR);
    free(image->tmpG);
    free(image->tmpB);
    free(image->rowSize);
    free(image->rowStart);
    free(image);
}

static void
ImageGetRow(ImageRec *image, unsigned char *buf, int y, int z) {
    unsigned char *iPtr, *oPtr, pixel;
    int count;

    if ((image->type & 0xFF00) == 0x0100) {
	fseek(image->file, (long)image->rowStart[y+z*image->ysize], SEEK_SET);
	fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
	      image->file);

	iPtr = image->tmp;
	oPtr = buf;
	for (;;) {
	    pixel = *iPtr++;
	    count = (int)(pixel & 0x7F);
	    if (!count) {
		return;
	    }
	    if (pixel & 0x80) {
		while (count--) {
		    *oPtr++ = *iPtr++;
		}
	    } else {
		pixel = *iPtr++;
		while (count--) {
		    *oPtr++ = pixel;
		}
	    }
	}
    } else {
	fseek(image->file, 512+(y*image->xsize)+(z*image->xsize*image->ysize),
	      SEEK_SET);
	fread(buf, 1, image->xsize, image->file);
    }
}

unsigned *
read_texture(char *name, unsigned int *width, unsigned int *height, int *components) {
    unsigned *base, *lptr;
    unsigned char *rbuf, *gbuf, *bbuf, *abuf;
    ImageRec *image;
    int y;

    image = ImageOpen(name);
    
    if(!image)
	return NULL;
    (*width)=image->xsize;
    (*height)=image->ysize;
    (*components)=image->zsize;
    base = (unsigned *)malloc(image->xsize*image->ysize*sizeof(unsigned));
    rbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    gbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    bbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    abuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    if(!base || !rbuf || !gbuf || !bbuf)
      return NULL;
    lptr = base;
    for(y=0; y<image->ysize; y++) {
	if(image->zsize>=4) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,gbuf,y,1);
	    ImageGetRow(image,bbuf,y,2);
	    ImageGetRow(image,abuf,y,3);
	    rgbatorgba(rbuf,gbuf,bbuf,abuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else if(image->zsize==3) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,gbuf,y,1);
	    ImageGetRow(image,bbuf,y,2);
	    rgbtorgba(rbuf,gbuf,bbuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else if(image->zsize==2) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,abuf,y,1);
	    latorgba(rbuf,abuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else {
	    ImageGetRow(image,rbuf,y,0);
	    bwtorgba(rbuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	}
    }
    ImageClose(image);
    free(rbuf);
    free(gbuf);
    free(bbuf);
    free(abuf);

    return (unsigned *) base;
}

void CreateGLTexture(tMaterial *mat)
{
	GLuint textureType = GL_RGB;
	if(mat->m_iBpp == 32)
		textureType = GL_RGBA;
	else if(mat->m_iBpp == 24)
		textureType = GL_RGB;

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &mat->m_id);

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, mat->m_id);
											
	glTexImage2D(GL_TEXTURE_2D, 0, mat->m_iBpp/8, mat->m_iWidth, mat->m_iHeight, 0, textureType, GL_UNSIGNED_BYTE, mat->m_cData);
					
	// If you want your own texture parameters, remove next two lines
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if( (!CheckSize(mat->m_iWidth)) && (!CheckSize(mat->m_iHeight))) // TEXTURE ISNT OF POWER OF 2
	{
		// CREATE MIPMAPS
		gluBuild2DMipmaps(GL_TEXTURE_2D, mat->m_iBpp, 
										 mat->m_iWidth, 
										 mat->m_iHeight,
										 textureType, 
										 GL_UNSIGNED_BYTE, 
										 mat->m_cData);
	}
	SAFEDEL(mat->m_cData);
}

/*
// Function Format - bool LoadJPG(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads Intel's JPEG Image
bool LoadJPG(char* filename, tMaterial *mat, bool asTexture)
{
	if(filename == NULL)
		return false;
		
	BOOL asAlpha = FALSE;
	DWORD width = 0, height = 0, channels = 0;
	GLuint RetVal;
		
	JPEG_CORE_PROPERTIES image;
	ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );
		

	if( ijlInit( &image ) != IJL_OK )
	{
		mat->m_iError = CANNOT_LOAD;
    	return false;
	}
        
	image.JPGFile = filename;
		
	if ((RetVal = ijlRead(&image, IJL_JFILE_READPARAMS)) == IJL_OK)
	{
		mat->m_iHeight = image.JPGHeight;
		mat->m_iWidth = image.JPGWidth;
		mat->m_iBpp = asAlpha ? 1 : 3;
		channels = mat->m_iBpp;
			
			
		mat->m_cData = new GLubyte[mat->m_iHeight * mat->m_iWidth * mat->m_iBpp];
		if (mat->m_cData)
		{
			image.DIBBytes = mat->m_cData;
			image.DIBColor = asAlpha ? IJL_G : IJL_RGB;
			image.DIBHeight = mat->m_iHeight;
			image.DIBWidth = mat->m_iWidth;
			image.DIBChannels = channels;
			
			if ((RetVal = ijlRead(&image,IJL_JFILE_READWHOLEIMAGE)) == IJL_OK)
			{
				strncpy(mat->m_strFilename, filename, 255);
		
				if(asTexture)
				{
					GLuint textureType = GL_RGB;
					if(mat->m_iBpp == 32)
						textureType = GL_RGBA;
					else if(mat->m_iBpp == 24)
						textureType = GL_RGB;

					// Generate a texture with the associative texture ID stored in the array
					glGenTextures(1, &mat->m_id);

					// Typical Texture Generation Using Data From The Bitmap
					glBindTexture(GL_TEXTURE_2D, mat->m_id);
															
					glTexImage2D(GL_TEXTURE_2D, 0, mat->m_iBpp/8, mat->m_iWidth, mat->m_iHeight, 0, textureType, GL_UNSIGNED_BYTE, mat->m_cData);
									
					// If you want your own texture parameters, remove next two lines
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					// CREATE MIPMAPS - A MUST FOR JPEGS
						gluBuild2DMipmaps(GL_TEXTURE_2D, mat->m_iBpp, 
														 mat->m_iWidth, 
														 mat->m_iHeight,
														 textureType, 
														 GL_UNSIGNED_BYTE, 
														 mat->m_cData);
					SAFEDEL(mat->m_cData);
				}
			}
			// delete img;
		}
		ijlFree(&image);
	}

	return true;
}
*/

// Function Format - bool LoadRGBA(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads plain RGBA Image	
bool LoadRGB(char* filename, tMaterial *mat, bool asTexture)
{
	if(filename == NULL)
		return false;

	glEnable(GL_TEXTURE_2D);  
	int Components;
		
	strncpy(mat->m_strFilename, filename, 255);

	mat->m_cData = (GLubyte *)read_texture("texture.rgb", &mat->m_iWidth, &mat->m_iHeight, &Components);
	
	if(asTexture)
		CreateGLTexture(mat);
	
	return true;
}

													
// Function Format - bool LoadTGA(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads plain comp/uncompressed TGA Image
bool LoadTGA(char* filename, tMaterial *mat, bool asTexture)
{   
	WORD Width = 0, Height = 0;			// The dimensions of the image
	GLubyte length = 0;					// The length in bytes to the pixels
	GLubyte imageType = 0;					// The image type (RLE, RGB, Alpha...)
	GLubyte bits = 0;						// The bits per pixel for the image (16, 24, 32)
	FILE *pFile = NULL;					// The file pointer
	int stride = 0;						// The stride (channels * Width)
	int i = 0;							// A counter
	GLuint channels;
		
	// This function loads in a TARGA (.TGA) file and returns its data to be
	// used as a texture or what have you.  This currently loads in a 16, 24
	// and 32-bit targa file, along with RLE compressed files.  Eventually you
	// will want to do more error checking to make it more robust.  This is
	// also a perfect start to go into a modular class for an engine.
	// Basically, how it works is, you read in the header information, then
	// move your file pointer to the pixel data.  Before reading in the pixel
	// data, we check to see the if it's an RLE compressed image.  This is because
	// we will handle it different.  If it isn't compressed, then we need another
	// check to see if we need to convert it from 16-bit to 24 bit.  24-bit and
	// 32-bit textures are very similar, so there's no need to do anything special.
	// We do, however, read in an extra bit for each color.

	// Open a file pointer to the targa file and check if it was found and opened 
	if((pFile = fopen(filename, "rb")) == NULL) 
	{
		mat->m_iError = FILE_NOT_FOUND;
		return false;
	}
			
	// Read in the length in bytes from the header to the pixel data
	fread(&length, sizeof(GLubyte), 1, pFile);
		
	// Jump over one GLubyte
	fseek(pFile,1,SEEK_CUR); 

	// Read in the imageType (RLE, RGB, etc...)
	fread(&imageType, sizeof(GLubyte), 1, pFile);
		
	// Skip past general information we don't care about
		fseek(pFile, 9, SEEK_CUR); 

		// Read the Width, Height and bits per pixel (16, 24 or 32)
		fread(&Width,  sizeof(WORD), 1, pFile);
		fread(&Height, sizeof(WORD), 1, pFile);
		fread(&bits,   sizeof(GLubyte), 1, pFile);
		
		// Now we move the file pointer to the pixel data
		fseek(pFile, length + 1, SEEK_CUR); 

		// Check if the image is RLE compressed or not
		if(imageType != TGA_RLE)
		{
			// Check if the image is a 24 or 32-bit image
			if(bits == 24 || bits == 32)
			{
				// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
				// Next, we calculate the stride and allocate enough memory for the pixels.
				channels = bits / 8;
				stride = channels * Width;
				mat->m_cData = new unsigned char[stride * Height];

				// Load in all the pixel data line by line
				for(int y = 0; y < Height; y++)
				{
					// Store a pointer to the current line of pixels
					unsigned char *pLine = &(mat->m_cData[stride * y]);

					// Read in the current line of pixels
					fread(pLine, stride, 1, pFile);
				
					// Go through all of the pixels and swap the B and R values since TGA
					// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
					for(i = 0; i < stride; i += channels)
					{
						int temp     = pLine[i];
						pLine[i]     = pLine[i + 2];
						pLine[i + 2] = temp;
					}
				}
			}
			// Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
			else if(bits == 16)
			{
				unsigned short pixels = 0;
				int r=0, g=0, b=0;

				// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
				// We then calculate the stride and allocate memory for the pixels.
				channels = 3;
				stride = channels * Width;
				mat->m_cData = new unsigned char[stride * Height];

				// Load in all the pixel data pixel by pixel
				for(int i = 0; i < Width*Height; i++)
				{
					// Read in the current pixel
					fread(&pixels, sizeof(unsigned short), 1, pFile);
					
					// To convert a 16-bit pixel into an R, G, B, we need to
					// do some masking and such to isolate each color value.
					// 0x1f = 11111 in binary, so since 5 bits are reserved in
					// each unsigned short for the R, G and B, we bit shift and mask
					// to find each value.  We then bit shift up by 3 to get the full color.
					b = (pixels & 0x1f) << 3;
					g = ((pixels >> 5) & 0x1f) << 3;
					r = ((pixels >> 10) & 0x1f) << 3;
					
					// This essentially assigns the color to our array and swaps the
					// B and R values at the same time.
					mat->m_cData[i * 3 + 0] = r;
					mat->m_cData[i * 3 + 1] = g;
					mat->m_cData[i * 3 + 2] = b;
				}
			}	
			// Else return a NULL for a bad or unsupported pixel format
			else
			{
				mat->m_iError = INVALID_PIXEL_FORMAT;
				return false;
			}
		}
		// Else, it must be Run-Length Encoded (RLE)
		else
		{
			// First, let me explain real quickly what RLE is.  
			// For further information, check out Paul Bourke's intro article at: 
			// http://astronomy.swin.edu.au/~pbourke/dataformats/rle/
			// 
			// Anyway, we know that RLE is a basic type compression.  It takes
			// colors that are next to each other and then shrinks that info down
			// into the color and a integer that tells how much of that color is used.
			// For instance:
			// aaaaabbcccccccc would turn into a5b2c8
			// Well, that's fine and dandy and all, but how is it down with RGB colors?
			// Simple, you read in an color count (rleID), and if that number is less than 128,
			// it does NOT have any optimization for those colors, so we just read the next
			// pixels normally.  Say, the color count was 28, we read in 28 colors like normal.
			// If the color count is over 128, that means that the next color is optimized and
			// we want to read in the same pixel color for a count of (colorCount - 127).
			// It's 127 because we add 1 to the color count, as you'll notice in the code.

			// Create some variables to hold the rleID, current colors read, channels, & stride.
			GLubyte rleID = 0;
			int colorsRead = 0;
			channels = bits / 8;
			stride = channels * Width;

			// Next we want to allocate the memory for the pixels and create an array,
			// depending on the channel count, to read in for each pixel.
			mat->m_cData = new unsigned char[stride * Height];
			GLubyte *pColors = new GLubyte [channels];

			// Load in all the pixel data
			while(i < Width*Height)
			{
				// Read in the current color count + 1
				fread(&rleID, sizeof(GLubyte), 1, pFile);
				
				// Check if we don't have an encoded string of colors
				if(rleID < 128)
				{
					// Increase the count by 1
					rleID++;

					// Go through and read all the unique colors found
					while(rleID)
					{
						// Read in the current color
						fread(pColors, sizeof(GLubyte) * channels, 1, pFile);

						// Store the current pixel in our image array
						mat->m_cData[colorsRead + 0] = pColors[2];
						mat->m_cData[colorsRead + 1] = pColors[1];
						mat->m_cData[colorsRead + 2] = pColors[0];

						// If we have a 4 channel 32-bit image, assign one more for the alpha
						if(bits == 32)
							mat->m_cData[colorsRead + 3] = pColors[3];

						// Increase the current pixels read, decrease the amount
						// of pixels left, and increase the starting index for the next pixel.
						i++;
						rleID--;
						colorsRead += channels;
					}
				}
				// Else, let's read in a string of the same character
				else
				{
					// Minus the 128 ID + 1 (127) to get the color count that needs to be read
					rleID -= 127;

					// Read in the current color, which is the same for a while
					fread(pColors, sizeof(GLubyte) * channels, 1, pFile);

					// Go and read as many pixels as are the same
					while(rleID)
					{
						// Assign the current pixel to the current index in our pixel array
						mat->m_cData[colorsRead + 0] = pColors[2];
						mat->m_cData[colorsRead + 1] = pColors[1];
						mat->m_cData[colorsRead + 2] = pColors[0];

						// If we have a 4 channel 32-bit image, assign one more for the alpha
						if(bits == 32)
							mat->m_cData[colorsRead + 3] = pColors[3];

						// Increase the current pixels read, decrease the amount
						// of pixels left, and increase the starting index for the next pixel.
						i++;
						rleID--;
						colorsRead += channels;
					}
					
				}
					
			}
		}

		// Close the file pointer that opened the file
		fclose(pFile);

		// Fill in our tImageTGA structure to pass back
		mat->m_iWidth = Width;
		mat->m_iHeight = Height;
		mat->m_iBpp = bits;
		strncpy(mat->m_strFilename, filename, 255);

		if(asTexture)
			CreateGLTexture(mat);

	return true;
}

// Function Format - bool LoadBMP(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads Windows BitMap Image	
bool LoadBMP(char* filename, tMaterial *mat, bool asTexture)
{
	HBITMAP hBmp = (HBITMAP)LoadImage(NULL, (LPCTSTR)filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if(hBmp == NULL)
		return false;

	BITMAP BP;
	GetObject(hBmp, sizeof(BP), (void *)&BP);

	mat->m_cData = new GLubyte[BP.bmHeight * BP.bmWidth * (BP.bmBitsPixel/8)];
	memcpy((void *)mat->m_cData, BP.bmBits, BP.bmHeight * BP.bmWidth * (BP.bmBitsPixel/8));
	
	mat->m_iHeight = BP.bmHeight;
	mat->m_iWidth = BP.bmWidth;
	mat->m_iBpp = BP.bmBitsPixel;

	strncpy(mat->m_strFilename, filename, 255);

	if(asTexture)
	{
		GLuint textureType = GL_BGR_EXT;
		if(mat->m_iBpp == 24)
			textureType = GL_BGR_EXT;
		else if (mat->m_iBpp == 32)
			textureType = GL_BGRA_EXT;

		// Generate a texture with the associative texture ID stored in the array
		glGenTextures(1, &mat->m_id);

		glPixelStorei(GL_PACK_ALIGNMENT, 4);
		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, mat->m_id);
												
		glTexImage2D(GL_TEXTURE_2D, 0, mat->m_iBpp/8, mat->m_iWidth, mat->m_iHeight, 0, textureType, GL_UNSIGNED_BYTE, mat->m_cData);
						
		// If you want your own texture parameters, remove next two lines
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if( (!CheckSize(mat->m_iWidth)) && (!CheckSize(mat->m_iHeight))) // TEXTURE ISNT OF POWER OF 2
		{
			// CREATE MIPMAPS
			gluBuild2DMipmaps(GL_TEXTURE_2D, mat->m_iBpp/8, 
											 mat->m_iWidth, 
											 mat->m_iHeight,
											 textureType, 
											 GL_UNSIGNED_BYTE, 
											 mat->m_cData);
		}
		SAFEDEL(mat->m_cData);
	}

	DeleteObject(hBmp);
	hBmp = NULL;

	return true;
}

// Function Format - bool LoadRAW(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads RAW Image
bool LoadRAW(char* filename, int iSize, tMaterial *mat, bool asTexture)
{
	int i=0;
	FILE *file = fopen(filename, "rb");

	if(file == NULL)
	{
		mat->m_iError = FILE_NOT_FOUND;
		return false;
	}

	mat->m_cData = new GLubyte[iSize*iSize];

	while(i<iSize*iSize)
	{
		mat->m_cData[i] = fgetc(file);
		i++;
	}

	fclose(file);

	mat->m_iBpp = 24;
			
	strncpy(mat->m_strFilename, filename, 255);

	if(asTexture)
		CreateGLTexture(mat);

	return true;
}

// Function Format - bool LoadPCX(char*, GLfloat, GLfloat)
//------------------------------------------------------------------
//  Description: This function loads PCX Image
bool LoadPCX(char* filename, tMaterial *mat, bool asTexture)
{
	FILE *File = NULL;
	GLubyte *pal = NULL;
	GLubyte *temp = NULL;
	GLubyte c;
	GLuint width, height;
	GLubyte *data;
	GLuint rep, off=0;
	GLuint p;
	unsigned char ImageID, Version, Enc, Bpp;
	GLuint x1, x2, y1, y2;

	File=fopen(filename,"rb");

	// check if we have a file open
	if(File == NULL) 
	{
		mat->m_iError = FILE_NOT_FOUND;
		return false;
	}
				
	ImageID = (GLubyte)fgetc(File);
	if(ImageID != 0x0A) 
	{
		mat->m_iError = INVALID_FILE_FORMAT_SPEC;
		fclose(File);
		return false;
	}

		Version = (GLubyte)fgetc(File);
		if(Version!= 5) 
		{
			mat->m_iError = INVALID_FILE_FORMAT_SPEC;
			fclose(File);
			return false;
		}

		Enc = (GLubyte)fgetc(File);

		Bpp = (GLubyte)fgetc(File);

		// get dimensions 
		x1 = fgetc(File);
		x1 |= fgetc(File)<<8;

		y1 = fgetc(File);
		y1 |= fgetc(File)<<8;

		x2 = fgetc(File);
		x2 |= fgetc(File)<<8;

		y2 = fgetc(File);
		y2 |= fgetc(File)<<8;

		
		// do assignments
		width = (x2-x1)+1;
		height= (y2-y1)+1;

		// go get the palette
		fseek(File,-769L,SEEK_END);

		// check palette bit and read it in
		if (fgetc(File) != 0xC) 
		{
			mat->m_iError = INVALID_PIXEL_FORMAT;
			fclose(File);
			return false;
		}

		pal= new GLubyte[768];
		for (p = 0; p < 768; p++)
			pal[p] = (GLubyte)getc(File);

		// go to encoded data
		fseek(File,128L,SEEK_SET);

		// alloc temp space for holding decoded data
		temp = (GLubyte *)malloc(width * height);

		// decode
		while((GLuint)off < (width * height)) 
		{
			c = (GLubyte)fgetc(File);
			if(c == EOF) 
			{
				mat->m_iError = CANNOT_LOAD;
				return false;
			}

			if((c & 0xC0) == 0xC0) 
			{
				rep = c & 0x3f;
				c = (GLubyte)fgetc(File);
				if(c == EOF) 
				{
					mat->m_iError = CANNOT_LOAD;
					return false;
				}

				for(GLuint i = 0; i < rep; i++)
				{
					temp[off++] = c;
				}
			}
			else
				temp[off++] = c;
		}

		// convert to 24 bit and flip
		data = ConvertPCX(temp, pal, width, height);

		// do clean up
		delete temp;
		delete pal;

		fclose(File);

		mat->m_iBpp = 24;
		mat->m_iWidth =width;
		mat->m_iHeight = height;
		mat->m_cData = data;


		GLuint textureType = GL_RGB;
			if(mat->m_iBpp == 32)
				textureType = GL_RGBA;
			else if(mat->m_iBpp == 24)
				textureType = GL_RGB;

		strncpy(mat->m_strFilename, filename, 255);

		if(asTexture)
			CreateGLTexture(mat);

	return true;
}

// ConvertPCX Utility function
GLubyte *ConvertPCX(GLubyte *dt, GLubyte *Palette, GLuint Width, GLuint Height)
{
	GLubyte *out;
	GLubyte *temp;
	GLuint i,j;
	
	out = new GLubyte[Width*3*Height];
	// no double counting
	memset(out, '0', sizeof(out));

	if(!out) {
		return NULL;
	}

	// do conversion now
	for(i = 0; i < Height*Width; i++) {
		out[(i)*3] = Palette[dt[i]*3];		// red
		out[(i)*3+1] = Palette[dt[i]*3+1];	// green
		out[(i)*3+2] = Palette[dt[i]*3+2];	// blue
	}

	// do flip - thanks Andre
	temp = new GLubyte[Width*3*Height];
	memcpy(temp, out, Width*3*Height);
	for(j = 0; j < Height; j++) {
		memcpy(&out[((Height-1) - j)*3*Width], &temp[j*3*Width], 3*Width);
	}
	delete temp;
	return out;
}

// Function Format - bool CheckSize(GLuint)
//------------------------------------------------------------------
//  Description: This function checks if number is of power of 2
bool CheckSize(GLuint Size)
{
	return	( (Size & ((~Size)+1)) == Size) ? true : false;
}


// Function Format - bool BitBlt(CMaterial, CMaterial,	
//								 GLshort, GLshort, GLshort,	GLshort,
//								 GLshort, GLshort, GLshort, GLshort)
//------------------------------------------------------------------
//  Description: This function BitBlits one material area onto another
bool BitBlt(			tMaterial src,	tMaterial dst,
						GLshort src_xstart, GLshort src_ystart,
						GLshort src_xend,	GLshort src_yend,
						GLshort dst_xstart, GLshort dst_ystart,
						GLshort blend,		GLshort alpha	)
{
	if(src.m_cData !=NULL && dst.m_cData !=NULL)
	{
		GLubyte *s, *d;								// Source & Destination

		int i, j, k;
		// Clamp Alpha If Value Is Out Of Range

		if( alpha > 255 ) alpha = 255;
		if( alpha < 0 ) alpha = 0;

		// Check For Incorrect Blend Flag Values
		if( blend < 0 ) blend = 0;
		if( blend > 1 ) blend = 1;



		d = dst.m_cData + (dst_ystart * dst.m_iWidth * dst.m_iBpp);  			// Start Row - dst (Row * Width In Pixels * Bytes Per Pixel)
		s = src.m_cData + (src_ystart * src.m_iWidth * src.m_iBpp);			// Start Row - src (Row * Width In Pixels * Bytes Per Pixel)

		for (i = 0 ; i < src_yend ; i++ )						// Height Loop
		{
			s = s + (src_xstart * src.m_iBpp);					// Move Through Src Data By Bytes Per Pixel
			d = d + (dst_xstart * dst.m_iBpp);					// Move Through Dst Data By Bytes Per Pixel

			for (j = 0 ; j < src_xend ; j++ )					// Width Loop
			{
				for( k = 0 ; (unsigned int)k < src.m_iBpp ; k++, d++, s++)			// "n" Bytes At A Time
				{
					if (blend)						// If Blending Is On

					*d = ( (*s * alpha) + (*d * (255-alpha)) ) >> 8;	// Multiply Src Data*alpha Add Dst Data*(255-alpha)
					else							// Keep in 0-255 Range With >> 8
					*d = *s;						// No Blending Just Do A Straight Copy
				}

			}
			d = d + (dst.m_iWidth - (src_xend + dst_xstart))*dst.m_iBpp;		// Add End Of Row
			s = s + (src.m_iWidth - (src_xend + src_xstart))*src.m_iBpp;		// Add End Of Row

		}

		return true;
	}
	return false;
}


bool isInString(char *string, const char *search) 
{
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);
	char *other;
	for (int i=0; i<len; i++) {
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {			// New Extension Begins Here!
			other=&string[i];
			pos=0;							// Begin New Search
			while (string[i]!='\n') {				// Search Whole Extension-String
				if (string[i]==search[pos]) pos++;		// Next Position
				if ((pos>maxpos) && string[i+1]=='\n') return true;	// We Have A Winner!
				i++;
			}
		}
	}
	return false;								// Sorry, Not Found!
}

void GetMaterial(float *p, FILE *file)
{
	char line[255];
	fgets(line, 255, file);
	sscanf(line, "%f %f %f %f\n", &p[0], &p[1], &p[2], &p[3]);
}

// Function Format - bool LoadTexture(char*, bool)
//------------------------------------------------------------------
//  Description: This function loads various image file formats
bool LoadTexture(char filename[], tMaterial *mat, bool asTexture)
{
    FILE *f;
    
    f = fopen(filename, "rb");
    if(!f) return 0;    
    fclose(f);
    
	if(filename !=NULL)
	{
		if(strstr(filename,".bmp") || strstr(filename, ".BMP"))
		{
			mat->m_bIsTex = asTexture;
			return LoadBMP(filename, mat, asTexture);
		}
		else
		if(strstr(filename, ".tga") || strstr(filename, ".TGA"))
		{
			mat->m_bIsTex = asTexture;
			return LoadTGA(filename, mat, asTexture); 
		}
		else
		if(strstr(filename, ".pcx") || strstr(filename, ".PCX"))
		{
			mat->m_bIsTex = asTexture;
			return LoadPCX(filename, mat, asTexture); 
		}	
		else
		if(strstr(filename, ".rgb") || strstr(filename, ".RGB"))
		{
			mat->m_bIsTex = asTexture;
			return LoadRGB(filename, mat, asTexture); 
		}
	/*	else
		if(strstr(filename, ".jpg") || strstr(filename, ".JPG") )
		{
			mat->m_bIsTex = asTexture;
			return LoadJPG(filename, mat, asTexture); 
		}*/
	}

	return false;
}

// Function Format - void ApplyMaterial(CMaterial *, MAT_RENDER)
//------------------------------------------------------------------
//  Description: This function sets state for selected material
void ApplyMaterial(tMaterial *mat, MAT_RENDER rendermode, GLfloat *color)
{
	if(mat !=NULL)
	{
		if(mat->m_id >= 0 && glIsTexture(mat->m_id) && mat->m_bIsTex)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mat->m_id); 
		}
		else
			glDisable(GL_TEXTURE_2D);

		if(rendermode == MAT_TEX)
		{
			glEnable(GL_LIGHTING);	
				glMaterialfv( GL_FRONT, GL_AMBIENT,	 mat->m_Ambient.m_rgba	);
				glMaterialfv( GL_FRONT, GL_DIFFUSE,	 mat->m_Diffuse.m_rgba	);
				glMaterialfv( GL_FRONT, GL_SPECULAR, mat->m_Specular.m_rgba );
				glMaterialfv( GL_FRONT, GL_EMISSION, mat->m_Emissive.m_rgba );
				glMaterialf( GL_FRONT, GL_SHININESS, mat->m_fShininess );
			
		}
		else if(rendermode == TEX) 
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0,1.0,1.0);
		}
		else if(rendermode == TEX_COLOR)
		{
			glDisable(GL_LIGHTING);
			if(color !=NULL)
				glColor3f(color[0], color[1], color[2]);
			else
				glColor3f(mat->m_Ambient.r, mat->m_Ambient.g, mat->m_Ambient.b); 
		}
		else
		{
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glMaterialfv( GL_FRONT, GL_AMBIENT,	 mat->m_Ambient.m_rgba	);
			glMaterialfv( GL_FRONT, GL_DIFFUSE,	 mat->m_Diffuse.m_rgba	);
			glMaterialfv( GL_FRONT, GL_SPECULAR, mat->m_Specular.m_rgba );
			glMaterialfv( GL_FRONT, GL_EMISSION, mat->m_Emissive.m_rgba );
			glMaterialf( GL_FRONT, GL_SHININESS, mat->m_fShininess );
		}
	}
	else
	{
		glDisable(GL_LIGHTING);
		if(color !=NULL)
			glColor3f(color[0], color[1], color[2]);
	}

}

bool LoadMaterial(tMaterial *mat, char *filename, FILE *f)
{
	if(mat == NULL)
		return false;

	FILE *file = NULL;
	
	if(f == NULL)
	{
		if(filename == NULL)
			return false;
		else
			file = fopen(filename,"r+t");
	}
	else
		file = f;
	
	if(file !=NULL)
	{
		fscanf(file, "TEXTURE %s ISTEX %d\n", mat->m_strFilename, &mat->m_bIsTex);
		LoadTexture(mat->m_strFilename, mat, mat->m_bIsTex);
				
		fscanf(file, "A %f %f %f %f\n", &mat->m_Ambient.r, &mat->m_Ambient.g, &mat->m_Ambient.b, &mat->m_Ambient.a);
		fscanf(file, "D %f %f %f %f\n", &mat->m_Diffuse.r, &mat->m_Diffuse.g, &mat->m_Diffuse.b, &mat->m_Diffuse.a);
		fscanf(file, "E %f %f %f %f\n", &mat->m_Emissive.r, &mat->m_Emissive.g, &mat->m_Emissive.b, &mat->m_Emissive.a);
		fscanf(file, "S %f %f %f %f SH %f\n", &mat->m_Specular.r, &mat->m_Specular.g, &mat->m_Specular.b, &mat->m_Specular.a, &mat->m_fShininess);
		fscanf(file, "ALPHA: %f %f %f %f\n", &mat->m_Alpha.r, &mat->m_Alpha.g, &mat->m_Alpha.b, &mat->m_Alpha.a);

		if(mat->m_Alpha.r !=-1 && mat->m_Alpha.g !=-1 && mat->m_Alpha.b !=-1)
			CreateAlpha(mat, mat->m_Alpha);
	}

	if(f == NULL)
		fclose(file);

	return true;
}

bool SaveMaterial(tMaterial *mat, char *filename, FILE *f)
{
	if(mat == NULL)
		return false;

	FILE *file = NULL;
	
	if(f == NULL)
	{
		if(filename == NULL)
			return false;
		else
			file = fopen(filename,"w+t");
	}
	else
		file = f;
	
	if(file !=NULL)
	{
		fprintf(file, "TEXTURE %s ISTEX %d\n", mat->m_strFilename, mat->m_bIsTex);

		fprintf(file, "A %f %f %f %f\n", mat->m_Ambient.r, mat->m_Ambient.g, mat->m_Ambient.b, mat->m_Ambient.a);
		fprintf(file, "D %f %f %f %f\n", mat->m_Diffuse.r, mat->m_Diffuse.g, mat->m_Diffuse.b, mat->m_Diffuse.a);
		fprintf(file, "E %f %f %f %f\n", mat->m_Emissive.r, mat->m_Emissive.g, mat->m_Emissive.b, mat->m_Emissive.a);
		fprintf(file, "S %f %f %f %f SH %f\n", mat->m_Specular.r, mat->m_Specular.g, mat->m_Specular.b, mat->m_Specular.a, mat->m_fShininess);
		fprintf(file, "ALPHA: %f %f %f %f\n", mat->m_Alpha.r, mat->m_Alpha.g, mat->m_Alpha.b, mat->m_Alpha.a);
	}

	if(f == NULL)
		fclose(file);

	return true;
}


bool SaveScreenshot(RECT Region, char *filename)
{

	// extract the height and width from the rectangle
	unsigned long Width = Region.right-Region.left,
    Height= Region.bottom-Region.top;

	long ImageSize = Width*Height*3;  // width*height*(bitdepth/bitsperbyte)

	unsigned char *Data = new unsigned char[ImageSize];  // allocate some space
	
	// read the data in RGB format from the color buffer
	glReadPixels(Region.left, Region.top, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, Data);

	FILE *Bitmap = fopen(filename, "wb");  // open up a file to write


	if(!Bitmap)  // bail if we couldn't open the file (shouldn't happen)
	{
		delete [] Data;
		return false;
	}

	
	unsigned char temp;
	
	// reorder RGB to standard bitmap ordering of BGR
	for(int index = 0; index < ImageSize; index+=3)
	{
		temp = Data[index];   
		Data[index] = Data[index+2]; // swap reds
		Data[index+2] = temp;        // with blues
	}

	// prepare the file header
	BITMAPFILEHEADER FileHeader;

	// clear everything out so we can fill arbitrarily
	memset(&FileHeader, 0, sizeof(BITMAPFILEHEADER));

	// filetype is always "BM"
	FileHeader.bfType = MAKEWORD('B','M'); 
	
	// total size of our bitmap file
	FileHeader.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + ImageSize;

	// offset from start of file to bitmap data
	// NOTE: this is misdocumented in the MSDN
	FileHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	// prepare the info header
	BITMAPINFOHEADER InfoHeader;

	// clear stuff
	memset(&InfoHeader, 0, sizeof(BITMAPINFOHEADER));

	// size of this structure
	InfoHeader.biSize = sizeof(BITMAPINFOHEADER); 

	// width and height
	InfoHeader.biWidth = Width;  
	InfoHeader.biHeight = Height;

	// number of image planes - must be 1 in Windows
	InfoHeader.biPlanes = 1;

	// bitdepth is 24-bit (3 GLubyte-sized components per pixel)
	InfoHeader.biBitCount = 24;

	// no compression - outut raw RGB data
	InfoHeader.biCompression = BI_RGB;

	// write to the file - yay.
	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, Bitmap);
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, Bitmap);
	fwrite(Data, sizeof(unsigned char), ImageSize, Bitmap);

	// perform some clean up
	fclose(Bitmap);
	delete [] Data;

	// it's all good.
	return true;
}


void CreateAlpha(tMaterial *mat, RGBA color)
{
	GLubyte *newData = new GLubyte[mat->m_iWidth * mat->m_iHeight * 4];
	GLubyte *oldData = new GLubyte[mat->m_iWidth * mat->m_iHeight * 3];

	GLuint textureType = GL_RGB;
	if(mat->m_iBpp == 32)
		textureType = GL_RGBA;
	else if(mat->m_iBpp == 24)
		textureType = GL_RGB;

	glGetTexImage(GL_TEXTURE_2D, 0, textureType, GL_UNSIGNED_BYTE, oldData);

	int t = 0;
	for (int i=0; i<mat->m_iWidth * mat->m_iHeight * mat->m_iBpp / 8; i+=3)
	{
		newData[t] = oldData[i];
		newData[t+1] = oldData[i+1];
		newData[t+2] = oldData[i+2];

		if(oldData[i] == color.r && oldData[i+1] == color.g && oldData[i+2] == color.b)
			newData[t+3] = 0;
		else
			newData[t+3] = 255;

		t+=4;
	}

	mat->m_cData = newData;
	mat->m_iBpp = 32;

	CreateGLTexture(mat);
	SAFEDEL(oldData);
}
