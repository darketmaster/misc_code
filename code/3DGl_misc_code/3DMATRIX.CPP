#include "3dmatrix.h"

#define FLOATSINCOS(x, s, c)  {(c) = cos(x); (s) = sin(x);}
#define floattan(x)           tan((x))

MATRIX_f identity_matrix_f = 
{
   {
      /* 3x3 identity */
      { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
   },

   /* zero translation */
   { 0.0, 0.0, 0.0 }
};

// get_translation_matrix_f:
//  Constructs a 3d translation matrix. When applied to the vector 
//  (vx, vy, vx), this will produce (vx+x, vy+y, vz+z).
void get_translation_matrix_f(MATRIX_f *m, float x, float y, float z)
{
   *m = identity_matrix_f;

   m->t[0] = x;
   m->t[1] = y;
   m->t[2] = z;
}


// get_scaling_matrix_f:
//  Constructs a 3d scaling matrix. When applied to the vector 
//  (vx, vy, vx), this will produce (vx*x, vy*y, vz*z).
void get_scaling_matrix_f(MATRIX_f *m, float x, float y, float z)
{
   *m = identity_matrix_f;

   m->v[0][0] = x;
   m->v[1][1] = y;
   m->v[2][2] = z;
}


// get_x_rotate_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around 
// the x axis by the specified amount 
void get_x_rotate_matrix_f(MATRIX_f *m, float r)
{
   float c, s;

   FLOATSINCOS(r, s, c);
   *m = identity_matrix_f;

   m->v[1][1] = c;
   m->v[1][2] = -s;

   m->v[2][1] = s;
   m->v[2][2] = c;
}

// get_y_rotate_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around 
//  the y axis by the specified amount
void get_y_rotate_matrix_f(MATRIX_f *m, float r)
{
   float c, s;

   FLOATSINCOS(r, s, c);
   *m = identity_matrix_f;

   m->v[0][0] = c;
   m->v[0][2] = s;

   m->v[2][0] = -s;
   m->v[2][2] = c;
}



// get_z_rotate_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around 
//  the z axis by the specified amount
void get_z_rotate_matrix_f(MATRIX_f *m, float r)
{
   float c, s;

   FLOATSINCOS(r, s, c);
   *m = identity_matrix_f;

   m->v[0][0] = c;
   m->v[0][1] = -s;

   m->v[1][0] = s;
   m->v[1][1] = c;
}

#define MAKE_ROTATION_f(x, y, z)                \
   float sin_x, cos_x;				\
   float sin_y, cos_y;				\
   float sin_z, cos_z;				\
   float sinx_siny, cosx_siny;			\
						\
   FLOATSINCOS(x, sin_x, cos_x);		\
   FLOATSINCOS(y, sin_y, cos_y);		\
   FLOATSINCOS(z, sin_z, cos_z);		\
						\
   sinx_siny = sin_x * sin_y;			\
   cosx_siny = cos_x * sin_y;

#define R00_f (cos_y * cos_z)
#define R10_f ((sinx_siny * cos_z) - (cos_x * sin_z))
#define R20_f ((cosx_siny * cos_z) + (sin_x * sin_z))

#define R01_f (cos_y * sin_z)
#define R11_f ((sinx_siny * sin_z) + (cos_x * cos_z))
#define R21_f ((cosx_siny * sin_z) - (sin_x * cos_z))

#define R02_f (-sin_y)
#define R12_f (sin_x * cos_y)
#define R22_f (cos_x * cos_y)

// get_rotation_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around
//  all three axis by the specified amounts
void get_rotation_matrix_f(MATRIX_f *m, float x, float y, float z)
{
   MAKE_ROTATION_f(x, y, z);

   m->v[0][0] = R00_f;
   m->v[0][1] = R01_f;
   m->v[0][2] = R02_f;

   m->v[1][0] = R10_f;
   m->v[1][1] = R11_f;
   m->v[1][2] = R12_f;

   m->v[2][0] = R20_f;
   m->v[2][1] = R21_f;
   m->v[2][2] = R22_f;

   m->t[0] = m->t[1] = m->t[2] = 0;
}

//get_align_matrix_f:
// Aligns a matrix along an arbitrary coordinate system.
void get_align_matrix_f(MATRIX_f *m, float xfront, float yfront, float zfront, float xup, float yup, float zup)
{
   float xright, yright, zright;

   normalize_vector_f(&xfront, &yfront, &zfront);
   normalize_vector_f(&xup, &yup, &zup);

   cross_product_f(xfront, yfront, zfront, xup, yup, zup, &xright, &yright, &zright);
   cross_product_f(xright, yright, zright, xfront, yfront, zfront, &xup, &yup, &zup);

   m->v[0][0] = xright; 
   m->v[0][1] = xup; 
   m->v[0][2] = xfront; 

   m->v[1][0] = yright;
   m->v[1][1] = yup;
   m->v[1][2] = yfront;

   m->v[2][0] = zright;
   m->v[2][1] = zup;
   m->v[2][2] = zfront;

   m->t[0] = m->t[1] = m->t[2] = 0;
}



// get_vector_rotation_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around
//  the specified x,y,z vector by the specified angle, in a clockwise direction.
void get_vector_rotation_matrix_f(MATRIX_f *m, float x, float y, float z, float a)
{
   float c, s, cc;

   FLOATSINCOS(a, s, c);
   cc = 1 - c;
   normalize_vector_f(&x, &y, &z);

   m->v[0][0] = (cc * x * x) + c;
   m->v[0][1] = (cc * x * y) + (z * s);
   m->v[0][2] = (cc * x * z) - (y * s);

   m->v[1][0] = (cc * x * y) - (z * s);
   m->v[1][1] = (cc * y * y) + c;
   m->v[1][2] = (cc * z * y) + (x * s);

   m->v[2][0] = (cc * x * z) + (y * s);
   m->v[2][1] = (cc * y * z) - (x * s);
   m->v[2][2] = (cc * z * z) + c;

   m->t[0] = m->t[1] = m->t[2] = 0;
}


// get_transformation_matrix_f:
//  Constructs a 3d transformation matrix, which will rotate points around
//  all three axis by the specified amounts (given in the Allegro fixed 
//  point, 256 degrees to a circle format), scale the result by the
//  specified amount (itofix(1) for no change of scale), and then translate
//  to the requested x, y, z position.
void get_transformation_matrix_f(MATRIX_f *m, float scale, float xrot, float yrot, float zrot, float x, float y, float z)
{
   MAKE_ROTATION_f(xrot, yrot, zrot);

   m->v[0][0] = R00_f * scale;
   m->v[0][1] = R01_f * scale;
   m->v[0][2] = R02_f * scale;

   m->v[1][0] = R10_f * scale;
   m->v[1][1] = R11_f * scale;
   m->v[1][2] = R12_f * scale;

   m->v[2][0] = R20_f * scale;
   m->v[2][1] = R21_f * scale;
   m->v[2][2] = R22_f * scale;

   m->t[0] = x;
   m->t[1] = y;
   m->t[2] = z;
}

void get_transform_matrix_f(MATRIX_f *m, float xrot, float yrot, float zrot, float x, float y, float z)
{
   MAKE_ROTATION_f(xrot, yrot, zrot);

   m->v[0][0] = R00_f;
   m->v[1][0] = R01_f;
   m->v[2][0] = R02_f;

   m->v[0][1] = R10_f;
   m->v[1][1] = R11_f;
   m->v[2][1] = R12_f;

   m->v[0][2] = R20_f;
   m->v[1][2] = R21_f;
   m->v[2][2] = R22_f;

   m->t[0] = x;
   m->t[1] = y;
   m->t[2] = z;
}

/* get_camera_matrix_f: 
 *  Constructs a camera matrix for translating world-space objects into
 *  a normalised view space, ready for the perspective projection. The
 *  x, y, and z parameters specify the camera position, xfront, yfront,
 *  and zfront is an 'in front' vector specifying which way the camera
 *  is facing (this can be any length: normalisation is not required),
 *  and xup, yup, and zup is the 'up' direction vector. Up is really only
 *  a 1.5d vector, since the front vector only leaves one degree of freedom
 *  for which way up to put the image, but it is simplest to specify it
 *  as a full 3d direction even though a lot of the information in it is
 *  discarded. The fov parameter specifies the field of view (ie. width
 *  of the camera focus) in fixed point, 256 degrees to the circle format.
 *  For typical projections, a field of view in the region 32-48 will work
 *  well. Finally, the aspect ratio is used to scale the Y dimensions of
 *  the image relative to the X axis, so you can use it to correct for
 *  the proportions of the output image (set it to 1 for no scaling).
 */
void get_camera_matrix_f(MATRIX_f *m, float x, float y, float z, float xfront, float yfront, float zfront, float xup, float yup, float zup, float fov, float aspect)
{
   MATRIX_f camera, scale;
   float xside, yside, zside, width, d;

   /* make 'in-front' into a unit vector, and negate it */
   normalize_vector_f(&xfront, &yfront, &zfront);
   xfront = -xfront;
   yfront = -yfront;
   zfront = -zfront;

   /* make sure 'up' is at right angles to 'in-front', and normalize */
   d = dot_product_f(xup, yup, zup, xfront, yfront, zfront);
   xup -= d * xfront; 
   yup -= d * yfront; 
   zup -= d * zfront;
   normalize_vector_f(&xup, &yup, &zup);

   /* calculate the 'sideways' vector */
   cross_product_f(xup, yup, zup, xfront, yfront, zfront, &xside, &yside, &zside);

   /* set matrix rotation parameters */
   camera.v[0][0] = xside; 
   camera.v[0][1] = yside;
   camera.v[0][2] = zside;

   camera.v[1][0] = xup; 
   camera.v[1][1] = yup;
   camera.v[1][2] = zup;

   camera.v[2][0] = xfront; 
   camera.v[2][1] = yfront;
   camera.v[2][2] = zfront;

   /* set matrix translation parameters */
   camera.t[0] = -(x*xside  + y*yside  + z*zside);
   camera.t[1] = -(x*xup    + y*yup    + z*zup);
   camera.t[2] = -(x*xfront + y*yfront + z*zfront);

   #define M_PI4   0.785398163397448309615660845819876
   /* construct a scaling matrix to deal with aspect ratio and FOV */
   width = floattan(M_PI4 - fov/2);
   get_scaling_matrix_f(&scale, width, -aspect*width, -1.0);
   #undef M_PI4

   /* combine the camera and scaling matrices */
   matrix_mul_f(&camera, &scale, m);   
}

// qtranslate_matrix_f:
//Adds a position offset to an existing matrix.
void qtranslate_matrix_f(MATRIX_f *m, float x, float y, float z)
{
   m->t[0] += x;
   m->t[1] += y;
   m->t[2] += z;
}

// qscale_matrix_f:
//  Adds a scaling factor to an existing matrix.
void qscale_matrix_f(MATRIX_f *m, float scale)
{
   int i, j;

   for (i=0; i<3; i++)
      for (j=0; j<3; j++)
	 m->v[i][j] *= scale;
}

void matrix_mul_f(MATRIX_f *m1, MATRIX_f *m2, MATRIX_f *out)
{
   int i, j;

   for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
	      out->v[i][j] = (m1->v[0][j] * m2->v[i][0]) + (m1->v[1][j] * m2->v[i][1]) +	(m1->v[2][j] * m2->v[i][2]);
      }

     out->t[i] = (m1->t[0] * m2->v[i][0]) +  (m1->t[1] * m2->v[i][1]) +  (m1->t[2] * m2->v[i][2]) +  m2->t[i];
   }
}

// vector_lengthf: 
// Computes the length of a vector, using the son of the squaw...
float vector_length_f(float x, float y, float z)
{
   return sqrt(x*x + y*y + z*z);
}

// normalize_vectorf: 
//  Converts the specified vector to a unit vector, which has the same
//  orientation but a length of one.
void normalize_vector_f(float *x, float *y, float *z)
{
   float length = 1.0 / vector_length_f(*x, *y, *z);

   *x *= length;
   *y *= length;
   *z *= length;
}

// cross_productf:
//  Calculates the cross product of two vectors.
void cross_product_f(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout)
{
   *xout = (y1 * z2) - (z1 * y2);
   *yout = (z1 * x2) - (x1 * z2);
   *zout = (x1 * y2) - (y1 * x2);
}

// polygon_z_normal_f:
//  Helper function for backface culling: returns the z component of the
//  normal vector to the polygon formed from the three vertices.
float polygon_z_normal_f(V3D_f *v1, V3D_f *v2, V3D_f *v3)
{
   return ((v2->x-v1->x) * (v3->y-v2->y)) - ((v3->x-v2->x) * (v2->y-v1->y));
}

float _persp_xscale_f = 160.0;
float _persp_yscale_f = 100.0;
float _persp_xoffset_f = 160.0;
float _persp_yoffset_f = 100.0;

// set_projection_viewport:
//  Sets the viewport used to scale the output of the persp_project() 
//  function.
void set_projection_viewport(int x, int y, int w, int h)
{

   _persp_xscale_f = w/2;
   _persp_yscale_f = h/2;
   _persp_xoffset_f = x + w/2;
   _persp_yoffset_f = y + h/2;
}

float inline dot_product_f(float x1, float y1, float z1, float x2, float y2, float z2)
{
   return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

void inline persp_project_f(float x, float y, float z, float *xout, float *yout)
{
   float z1 = 1.0f / z;
   *xout = ((x * z1) * _persp_xscale_f) + _persp_xoffset_f;
   *yout = ((y * z1) * _persp_yscale_f) + _persp_yoffset_f;
}

// apply_matrix_f:
//  Floating point vector by matrix multiplication routine.
void apply_matrix_f(MATRIX_f *m, float x, float y, float z,
		    float *xout, float *yout, float *zout)
{
#define CALC_ROW(n) (x * m->v[(n)][0] + y * m->v[(n)][1] + z * m->v[(n)][2] + m->t[(n)])
   *xout = CALC_ROW(0);
   *yout = CALC_ROW(1);
   *zout = CALC_ROW(2);
#undef CALC_ROW
}

// inverse_apply_matrix_f:
//  Floating point vector by matrix multiplication routine.
void inverse_apply_matrix_f(MATRIX_f *m, float x, float y, float z,
		    float *xout, float *yout, float *zout)
{
#define CALC_ROW(n) (x * m->v[0][(n)] + y * m->v[1][(n)] + z * m->v[2][(n)] - m->t[(n)])
   *xout = CALC_ROW(0);
   *yout = CALC_ROW(1);
   *zout = CALC_ROW(2);
#undef CALC_ROW
}

////////////////////////////////////quaternions routines//////////////////////////

QUAT identity_quat = { 1.0, 0.0, 0.0, 0.0 };

#define EPSILON (0.001)

/* get_rotation_quat:
 *  Constructs a quaternion which will rotate points around all three axis by
 *  the specified amounts (given in binary, 360 degrees to a circle format).
 */
void my_get_rotation_quat(QUAT *q, float x, float y, float z)
{
   float sx;
   float sy;
   float sz;
   float cx;
   float cy;
   float cz;
   float cycz;
   float sysz;

   FLOATSINCOS(x*0.5, sx, cx);
   FLOATSINCOS(y*0.5, sy, cy);
   FLOATSINCOS(z*0.5, sz, cz);

   sysz = sy * sz;
   cycz = cy * cz;

   q->w = (cx * cycz) + (sx * sysz);
   q->x = (sx * cycz) - (cx * sysz);
   q->y = (cx * sy * cz) + (sx * cy * sz);
   q->z = (cx * cy * sz) - (sx * sy * cz);
}

/* quat_to_matrix:
 * Constructs a rotation matrix from a quaternion.
 */
void my_quat_to_matrix(QUAT *q, MATRIX_f *m)
{
   float ww;
   float xx;
   float yy;
   float zz;
   float wx;
   float wy;
   float wz;
   float xy;
   float xz;
   float yz;

  /* This is the layout for converting the values in a quaternion to a
   * matrix.
   *
   *  | ww + xx - yy - zz       2xy + 2wz             2xz - 2wy     |
   *  |     2xy - 2wz       ww - xx + yy - zz         2yz - 2wx     |
   *  |     2xz + 2wy           2yz - 2wx         ww + xx - yy - zz |
   */

   ww = q->w * q->w;
   xx = q->x * q->x;
   yy = q->y * q->y;
   zz = q->z * q->z;
   wx = q->w * q->x * 2;
   wy = q->w * q->y * 2;
   wz = q->w * q->z * 2;
   xy = q->x * q->y * 2;
   xz = q->x * q->z * 2;
   yz = q->y * q->z * 2;

   m->v[0][0] = ww + xx - yy - zz;
   m->v[0][1] = xy - wz;
   m->v[0][2] = xz + wy;

   m->v[1][0] = xy + wz;
   m->v[1][1] = ww - xx + yy - zz;
   m->v[1][2] = yz - wx;

   m->v[2][0] = xz - wy;
   m->v[2][1] = yz + wx;
   m->v[2][2] = ww - xx - yy + zz;

   m->t[0] = 0.0;
   m->t[1] = 0.0;
   m->t[2] = 0.0;
}

#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))

/* quat_slerp:
 *  Constructs a quaternion that represents a rotation between 'from' and
 *  'to'. The argument 't' can be anything between 0 and 1 and represents
 *  where between from and to the result will be.  0 returns 'from', 1
 *  returns 'to', and 0.5 will return a rotation exactly in between. The
 *  result is copied to 'out'.
 *
 *  The variable 'how' can be any one of the following:
 *
 *      QUAT_SHORT - This equivalent quat_interpolate, the rotation will
 *                   be less than 180 degrees
 *      QUAT_LONG  - rotation will be greater than 180 degrees
 *      QUAT_CW    - rotation will be clockwise when viewed from above
 *      QUAT_CCW   - rotation will be counterclockwise when viewed
 *                   from above
 *      QUAT_USER  - the quaternions are interpolated exactly as given
 */
void my_quat_slerp(QUAT *from, QUAT *to, float t, QUAT *out, int how)
{
   QUAT   to2;
   double angle;
   double cos_angle;
   double scale_from;
   double scale_to;
   double sin_angle;

   cos_angle = (from->x * to->x) +
	       (from->y * to->y) +
	       (from->z * to->z) +
	       (from->w * to->w);

   if (((how == QUAT_SHORT) && (cos_angle < 0.0)) ||
       ((how == QUAT_LONG)  && (cos_angle > 0.0)) ||
       ((how == QUAT_CW)    && (from->w > to->w)) ||
       ((how == QUAT_CCW)   && (from->w < to->w))) {
      cos_angle = -cos_angle;
      to2.w     = -to->w;
      to2.x     = -to->x;
      to2.y     = -to->y;
      to2.z     = -to->z;
   }
   else {
      to2.w = to->w;
      to2.x = to->x;
      to2.y = to->y;
      to2.z = to->z;
   }

   if ((1.0 - ABS(cos_angle)) > EPSILON) {
      /* spherical linear interpolation (SLERP) */
      angle = acos(cos_angle);
      sin_angle  = sin(angle);
      scale_from = sin((1.0 - t) * angle) / sin_angle;
      scale_to   = sin(t         * angle) / sin_angle;
   }
   else {
      /* to prevent divide-by-zero, resort to linear interpolation */
      scale_from = 1.0 - t;
      scale_to   = t;
   }

   out->w = (float)((scale_from * from->w) + (scale_to * to2.w));
   out->x = (float)((scale_from * from->x) + (scale_to * to2.x));
   out->y = (float)((scale_from * from->y) + (scale_to * to2.y));
   out->z = (float)((scale_from * from->z) + (scale_to * to2.z));
}

