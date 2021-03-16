#include "camera.h"
#include <alleggl.h>
#include <GL/glu.h>
#include <stdio.h>


CAMERA::CAMERA ()
{
 set_var(320, 240, 48, 1);
 orientation = identity_quat;
}

CAMERA::CAMERA (int vpw_, int vph_, int fov_, float asp_)
{
 set_var(vpw_, vph_, fov_, asp_);
 xoffset = 0;
 yoffset = 0;
 
 orientation = identity_quat;
}

CAMERA::CAMERA (int vpw_, int vph_, int fov_, float asp_, float x, float y, float z, float heading,float  pitch,float  roll) 
{

 set_var(vpw_, vph_, fov_, asp_);
 xoffset = 0;
 yoffset = 0;
 
 set_pos(x, y, z);
 
 set_ori(heading,  pitch,  roll);
 
}

CAMERA::~CAMERA ()
{
}

void CAMERA::set_var(int vpw_, int vph_, int fov_, float asp_)
{
 viewport_w = vpw_;
 viewport_h = vph_;
 fov = fov_;
 aspect = asp_;
 per = (float)viewport_w/(float)viewport_h;
}

void CAMERA::set_pos(float x, float y, float z)
{
 position.x = x;
 position.y = y;
 position.z = z;
}

void CAMERA::set_ori(float heading,float  pitch,float  roll)
{
 QUAT q;
 CVector3 vect;
 orientation = identity_quat;
 
 //heading
 get_y_rotate_quat(&q, heading);
 quat_mul(&orientation, &q, &orientation); 
 //pitch
 apply_quat(&orientation, 1, 0, 0, &vect.x, &vect.y, &vect.z);
 get_vector_rotation_quat(&q, vect.x, vect.y, vect.z, pitch);
 quat_mul(&orientation, &q, &orientation);
 //roll
 apply_quat(&orientation, 0, 0, 1, &vect.x, &vect.y, &vect.z);
 get_vector_rotation_quat(&q, vect.x, vect.y, vect.z, roll);
 quat_mul(&orientation, &q, &orientation);
}


void CAMERA::draw_overlay() {
	float heading, pitch, roll;
	int color;
	CVector3 v;

	glViewport(0, 0, SCREEN_W, SCREEN_H);
	
	glLoadIdentity();

	/* Overlay some text describing the current situation */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(48, 1.333333, 1.0, 120.0);
	glMatrixMode(GL_MODELVIEW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	color = 0;
	glScalef(.1, .1, .1);
	allegro_gl_printf(agl_font, -20, 15.0, -34, color, "Viewport width:  %03d pix (w/W changes)", viewport_w);
	allegro_gl_printf(agl_font, -20, 14.3, -34, color, "Viewport height: %03d pix (h/H changes)", viewport_h);
	allegro_gl_printf(agl_font, -20, 13.6, -34, color, "Field Of View:    %02d deg (f/F changes)", fov);
	allegro_gl_printf(agl_font, -20, 12.9, -34, color, "Aspect Ratio:    %.2f   (a/A changes)", aspect);
	allegro_gl_printf(agl_font, -20, 12.2, -34, color, "X position:     %+.2f (x/X changes)", position.x);
	allegro_gl_printf(agl_font, -20, 11.5, -34, color, "Y position:     %+.2f (y/Y changes)", position.y);
	allegro_gl_printf(agl_font, -20, 10.8, -34, color, "Z position:     %+.2f (z/Z changes)", position.z);

	/* Convert the orientation QUAT into heading, pitch and roll to display */
	convert_quat(&orientation, &heading, &pitch, &roll);

	allegro_gl_printf(agl_font, -20, 10.1, -34, color, "Heading:        %+.2f deg (left/right changes)", RAD2DEG(heading));
	allegro_gl_printf(agl_font, -20,  9.4, -34, color, "Pitch:          %+.2f deg (pgup/pgdn changes)", RAD2DEG(pitch));
	allegro_gl_printf(agl_font, -20,  8.7, -34, color, "Roll:           %+.2f deg (r/R changes)", RAD2DEG(roll));
	apply_quat(&orientation, 0, 0, -1, &v.x, &v.y, &v.z);	
	allegro_gl_printf(agl_font, -20,  8.0, -34, color, "Front Vector:    %.2f, %.2f, %.2f", v.x, v.y, v.z);
	apply_quat(&orientation, 0, 1, 0, &v.x, &v.y, &v.z);	
	allegro_gl_printf(agl_font, -20,  7.3, -34, color, "Up Vector:       %.2f, %.2f, %.2f", v.x, v.y, v.z);

	allegro_gl_printf(agl_font, -20,  6.6, -34, color, "QUAT:  %f, %f, %f, %f ", orientation.w, orientation.x, orientation.y, orientation.z);
	
	
	glBlendFunc(GL_ONE, GL_ZERO);

	glScalef(10, 10, 10);
	glEnable(GL_DEPTH_TEST);
}


///////
/* deal with user input */
void CAMERA::process_input(void) {

	QUAT q;

	poll_keyboard();

	if (key[KEY_W]) {
		if (key_shifts & KB_SHIFT_FLAG) {
			if (viewport_w < SCREEN_W)
				viewport_w += 8;
		}
		else { 
			if (viewport_w > 16)
				viewport_w -= 8;
		}
	}

	if (key[KEY_H]) {
		if (key_shifts & KB_SHIFT_FLAG) {
			if (viewport_h < SCREEN_H)
				viewport_h += 8;
		}
		else {
			if (viewport_h > 16)
				viewport_h -= 8;
		}
	}

	if (key[KEY_X]) {
		if (key_shifts & KB_SHIFT_FLAG)
			position.x += 0.5;
		else
			position.x -= 0.5;
	}

	if (key[KEY_Y]) {
		if (key_shifts & KB_SHIFT_FLAG)
			position.y += 0.5;
		else
			position.y -= 0.5;
	}

	if (key[KEY_Z]) {
		if (key_shifts & KB_SHIFT_FLAG)
			position.z += 0.5;
		else
			position.z -= 0.5;
	}
	
	if (key[KEY_UP]) {
		CVector3 front;
		/* Note: We use -1 here because Allegro's coordinate system
		 * is slightly different than OpenGL's.
		 */
		apply_quat(&orientation, 0, 0, -1, &front.x, &front.y, &front.z);
		position.x += front.x;
		position.y += front.y;
		position.z += front.z;
	}
	if (key[KEY_DOWN]) {
		CVector3 front;
		apply_quat(&orientation, 0, 0, -1, &front.x, &front.y, &front.z);
		position.x -= front.x;
		position.y -= front.y;
		position.z -= front.z;
	}


	/* When turning right or left, we only want to change the heading.
	 * That is, we only want to rotate around the absolute Y axis
	 */
	if (key[KEY_LEFT]) {
		get_y_rotate_quat(&q, -1);
		quat_mul(&orientation, &q, &orientation);
	}
	if (key[KEY_RIGHT]) {
		get_y_rotate_quat(&q, 1);
		quat_mul(&orientation, &q, &orientation);
	}

	/* However, when rolling or changing pitch, we do a rotation relative to
	 * the current orientation of the camera. This is why we extract the
	 * 'right' and 'front' vectors of the camera and apply a rotation on
	 * those.
	 */
	if (key[KEY_PGUP]) {
		CVector3 right;
		apply_quat(&orientation, 1, 0, 0, &right.x, &right.y, &right.z);
		get_vector_rotation_quat(&q, right.x, right.y, right.z, -1);
		quat_mul(&orientation, &q, &orientation);
	}
	if (key[KEY_PGDN]) {
		CVector3 right;
		apply_quat(&orientation, 1, 0, 0, &right.x, &right.y, &right.z);
		get_vector_rotation_quat(&q, right.x, right.y, right.z, 1);
		quat_mul(&orientation, &q, &orientation);
	}

	if (key[KEY_R]) {
		CVector3 front;
		apply_quat(&orientation, 0, 0, 1, &front.x, &front.y, &front.z);

		if (key_shifts & KB_SHIFT_FLAG)
			get_vector_rotation_quat(&q, front.x, front.y, front.z, -1);
		else
			get_vector_rotation_quat(&q, front.x, front.y, front.z, 1);

		quat_mul(&orientation, &q, &orientation);
	}
	
	if (key[KEY_F]) {
		if (key_shifts & KB_SHIFT_FLAG) {
			if (fov < 96)
				fov++;
		}
		else {
			if (fov > 16)
				fov--;
		}
	}

	if (key[KEY_A]) {
		if (key_shifts & KB_SHIFT_FLAG) {
			aspect += 0.05;
			if (aspect > 2)
				aspect = 2;
		}
		else {
			aspect -= 0.05;
			if (aspect < .1)
				aspect = .1;
		}
	}
}

/* Sets up the camera for displaying the world */
void CAMERA::set_camera() {
	float theta;

    glViewport( xoffset, yoffset, viewport_w, viewport_h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((float)fov, aspect * per, 1.0, 512.0);//512.0);//512.0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    allegro_gl_apply_quat(&orientation);   
	glTranslatef(-position.x, -position.y, -position.z);

}

