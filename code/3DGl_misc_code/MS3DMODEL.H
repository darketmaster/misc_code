#ifndef _MS3DMODEL_H_
#define _MS3DMODEL_H_

#include <stdio.h>
#include <GL/gl.h>

#include "3DMATRIX.H"

/* MilkShape constants */
#define MS_MAX_NAME             32
#define MS_MAX_PATH             256

class Tri
{
public:
	int		v[3];	// indices into vertices[]
	int		n[3];	// indices into normals[], normal per vertex
};

class Shape 
{
	public:
		/*	Constructor. */
		Shape();

		/*	Destructor. */
		~Shape();

		bool	loadFromFile( const char *filename );
		bool    loadFromMs3dAsciiSegment( FILE *file );

		bool	saveToFile( const char *filename );
		void	render( void );

//	private:
		int	num_vertices;
		V3D_f	*vertices;

		int	num_triangles;
		Tri	*triangles;

		int	num_normals;
		CVector3	*normals;
};


class Material
{
public:
	Material();
	~Material();
	bool loadFromMs3dAsciiSegment( FILE *file );
	void activate( void );
	void reloadTexture( void );

private:
	char  Name[MS_MAX_NAME];
	float Ambient[4];
	float Diffuse[4];
	float Specular[4];
	float Emissive[4];
	float Shininess;
	float Transparency;
	char  DiffuseTexture[MS_MAX_NAME];
	char  AlphaTexture[MS_MAX_NAME];
	GLuint texture;
	GLuint Atexture;
};

class KeyFrame
{
	public:
		float	Time;
		float	Value[3];
};

class Bone
{
	public:
		Bone();
		~Bone();
		bool	loadFromMs3dAsciiSegment( FILE *file );
		void	render( void );
		void	fixup( void );
		void	advanceTo( float time );
		void	initialize( void );
//	private:
		char	Name[MS_MAX_NAME];
		char	ParentName[MS_MAX_NAME];
		Bone	*Parent;						// pointer to parent bone (or NULL)
		float	startPosition[3];
		float	startRotation[3];
		MATRIX_f	m_relative;				// fixed transformation matrix relative to parent 
		MATRIX_f	m_final;				// absolute in accordance to animation
		MATRIX_f    m_absolute;
		int		NumPositionKeys;
		KeyFrame *PositionKeyFrames;
		int		NumRotationKeys;
		KeyFrame *RotationKeyFrames;
};

class Model
{
	public:
		Model();
		~Model();

		bool    loadFromMs3dAsciiFile( const char *filename );

		void	reloadTextures( void );
		void	render( void );
		void	renderBones( void );
		bool	linkBones( void );
		void	initializeBones( void );
		void	advanceAnimation( float deltaTime );
		void	attachSkin( void );
		void    UpdateMaterials();
//	private:
		float	MaxTime;		// length of animation (= Frames)
		float	CurrentTime;	// advancement within animation

		int		num_shapes;
		Shape	*shapes;
		int		*material_indices;

		int		num_materials;
		Material *materials;

		int		num_bones;
		Bone	*bones;
};


#endif          /* ifndef _MS3DMODEL_H_ */
	
