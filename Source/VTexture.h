#ifndef __VTEXTURE
#define __VTEXTURE

class VTexture
{
public:
	virtual bool		Create();
	virtual bool		Destroy();

	GLuint				mHandle;
	GLuint				mSize[2];
};

class VTestTexture : public VTexture
{
public:
	bool				Create();
};

class VDepthTexture : public VTexture
{
public:
	bool				Create();
};

class VPVRTexture : public VTexture
{
public:
	bool				Create(); // don't call this
	VResult				Create(const char *fileName);
};

#endif