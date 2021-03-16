#include "textout.h"
#include "log.h"
#include "timers.h"
#include "models.h"

////////////////////////////////////////////////////////////////////////////////////

GLuint init_cfont(float _xoff)							// Build Our Font Display List
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord
    GLuint	loop;
    GLuint	base;
    
	base=glGenLists(256);								// Creating 256 Display Lists
	for (loop=0; loop<256; loop++)						// Loop Through All 256 Lists
	{
		cx=((float)(loop%16))/16.0f;						// X Position Of Current Character
		cy=((float)(loop/16))/16.0f;						// Y Position Of Current Character
		glNewList(base+loop,GL_COMPILE);				// Start Building A List
			glBegin(GL_QUADS);							// Use A Quad For Each Character
				glTexCoord2f(cx,1-cy);			        // Texture Coord (Bottom Left)
				glVertex2i(0,0);						// Vertex Coord (Bottom Left)
    			glTexCoord2f(cx,1-cy-0.0625f);   		// Texture Coord (Top Left)
				glVertex2i(0,1);						// Vertex Coord (Top Left)
				glTexCoord2f(cx+0.0625f,1-cy-0.0625f);	// Texture Coord (Top Right)
				glVertex2i(1,1);                		// Vertex Coord (Top Right)
				glTexCoord2f(cx+0.0625f,1-cy);	        // Texture Coord (Bottom Right)
				glVertex2i(1,0);						// Vertex Coord (Bottom Right)
			glEnd();									// Done Building Our Quad (Character)
			glTranslatef(_xoff,0.0,0.0);				// Move To The Right Of The Character 0.7
		glEndList();
	}
    return base;
}

void destroy_cfont(GLuint	base)// Delete The Font From Memory
{
	glDeleteLists(base,256);// Delete All 256 Display Lists
}

void print_text(GLuint base, int TEXT_HEIGHT,float x, float y, const char *fmt, ...)	
{
	char text[1024];
	va_list		ap;	

	if (fmt == NULL)
		return;	

	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);
	
	glLoadIdentity();
	glTranslated(x, y, 0);
	glScalef(TEXT_HEIGHT, TEXT_HEIGHT, TEXT_HEIGHT);
	
	glListBase(base-32);						
	glCallLists(strlen(text),GL_BYTE,text);
	glScalef(1, 1, 1);
}


///////////////////////////////////////////////////////////////////////////
//                           ortho function
///////////////////////////////////////////////////////////////////////////

void OrthoMode(int left, int top, int right, int bottom)
{
	// This function takes in rectangle coordinates for our 2D view port.
	// What happens below is, we need to change our projection matrix.  To do this,
	// we call glMatrixMode(GL_PROJECTION) to tell OpenGL the current mode we want.
	// Then we push on a new matrix so we can just call glPopMatrix() to return us
	// to our previous projection matrix, but first we will render in ortho mode.
	//  Next, we load a new identity matrix so that everything is initialize before
	// we go into ortho mode.  One we switch to ortho by calling glOrth(), we then
	// want to set our matrix mode to GL_MODELVIEW, which puts us back and ready to
	// render our world using the model matrix.  We also initialize this new model view
	// matrix for our 2D rendering before drawing anything.

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	glLoadIdentity();
	glOrtho( left, right, bottom, top, 0, 1 );	
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
 	glDisable(GL_DEPTH_TEST);
  	glDisable(GL_LIGHTING);

}

void PerspectiveMode()
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

