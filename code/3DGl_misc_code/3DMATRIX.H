#ifndef _3DMATRIX_H_
#define _3DMATRIX_H_

#include "float.h"
#include "math.h"
#include "3dvector.h"

#ifdef __cplusplus
   extern "C" {
#endif

/* Define M_PI in case the compiler doesn't */
#ifndef M_PI
   #define M_PI   3.1415926535897932384626433832795
#endif

typedef struct MATRIX_f          /* transformation matrix (floating point) */
{
   float v[3][3];                /* scaling and rotation */
   float t[3];                   /* translation */
} MATRIX_f;

float inline dot_product_f(float x1, float y1, float z1, float x2, float y2, float z2);

void inline persp_project_f(float x, float y, float z, float *xout, float *yout);

extern MATRIX_f identity_matrix_f;

void get_translation_matrix_f(MATRIX_f *m, float x, float y, float z);

void get_scaling_matrix_f(MATRIX_f *m, float x, float y, float z);
 
void get_x_rotate_matrix_f(MATRIX_f *m, float r);

void get_y_rotate_matrix_f(MATRIX_f *m, float r);

void get_z_rotate_matrix_f(MATRIX_f *m, float r);

void get_rotation_matrix_f(MATRIX_f *m, float x, float y, float z);

void get_align_matrix_f(MATRIX_f *m, float xfront, float yfront, float zfront, float xup, float yup, float zup);

void get_vector_rotation_matrix_f(MATRIX_f *m, float x, float y, float z, float a);

void get_transformation_matrix_f(MATRIX_f *m, float scale, float xrot, float yrot, float zrot, float x, float y, float z);

void get_transform_matrix_f(MATRIX_f *m, float xrot, float yrot, float zrot, float x, float y, float z);

void get_camera_matrix_f(MATRIX_f *m, float x, float y, float z, float xfront, float yfront, float zfront, float xup, float yup, float zup, float fov, float aspect);

void qtranslate_matrix_f(MATRIX_f *m, float x, float y, float z);

void qscale_matrix_f(MATRIX_f *m, float scale);

void matrix_mul_f(MATRIX_f *m1, MATRIX_f *m2, MATRIX_f *out);

float vector_length_f(float x, float y, float z);

void normalize_vector_f(float *x, float *y, float *z);

void cross_product_f(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout);

float polygon_z_normal_f(V3D_f *v1, V3D_f *v2, V3D_f *v3);

void set_projection_viewport(int x, int y, int w, int h);

void apply_matrix_f(MATRIX_f *m, float x, float y, float z,
		    float *xout, float *yout, float *zout);

void inverse_apply_matrix_f(MATRIX_f *m, float x, float y, float z,
		    float *xout, float *yout, float *zout);

typedef struct QUAT
{
     float w, x, y, z;
} QUAT;

extern QUAT identity_quat;		    
		    
#define QUAT_SHORT   0
#define QUAT_LONG    1
#define QUAT_CW      2
#define QUAT_CCW     3
#define QUAT_USER    4

#define my_quat_interpolate(from, to, t, out)   my_quat_slerp((from), (to), (t), (out), QUAT_SHORT);		    

void my_get_rotation_quat(QUAT *q, float x, float y, float z);
    
void my_quat_to_matrix(QUAT *q, MATRIX_f *m);

void my_quat_slerp(QUAT *from, QUAT *to, float t, QUAT *out, int how);



#ifdef __cplusplus
   }
#endif

#endif          /* ifndef _3DMATRIX_H_ */


