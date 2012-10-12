#ifndef __VBO
#define __VBO

#include <vector>
#include "VShader.h"
#include "VTrajectory.h"

class VVertexBufferObject
{
public:
	virtual bool		Create();
	virtual bool		Destroy();
	virtual void		Fill() = 0;
	virtual void		Draw(const VProgram &program) = 0;

	GLuint				mHandle;
	GLuint				mStride;
	GLuint				mVertexCount;
};

class VPlaneVBO : public VVertexBufferObject
{
public:
	virtual void		Fill();
	virtual void		Draw(const VProgram &program);
};

class VSphereVBO : public VVertexBufferObject
{
public:
	/* constructor */	VSphereVBO();
	virtual void		Fill();
	virtual void		Draw(const VProgram &program);

	GLuint				mQuality;
};

struct VVertexData
{
	glm::vec3			mPosition;
	glm::vec3			mNormal;
	glm::vec2			mTexCoord;
};

typedef std::vector<GLushort> VIndexList;
typedef std::vector<VVertexData> VVertexList;

class VLinesVBO : public VVertexBufferObject
{
public:
	virtual void		Fill();
	virtual void		Draw(const VProgram &program);

	VPointList			mPoints;
};

class VTubeVBO : public VVertexBufferObject
{
public:
	virtual bool		Create();
	virtual bool		Destroy();
	virtual void		Fill();
	virtual void		Draw(const VProgram &program);

	GLuint				mIndexHandle;
	GLuint				mIndexCount;
	VPointList			mCtrlPoints;
	GLfloat				mRadius;
};

class VShipVBO : public VVertexBufferObject
{
public:
	virtual void		Fill();
	virtual void		Draw(const VProgram &program);
};

inline VSphereVBO::VSphereVBO()
{
	mQuality = 2;
}

#endif