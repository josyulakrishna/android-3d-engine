#ifndef __VAPP
#define __VAPP

#include "VShader.h"
#include "VTexture.h"
#include "VVertexBufferObject.h"
#include "VTrajectory.h"

enum VGameState
{
	ST_PAUSED,
	ST_GAMEPLAY
};

class VApp
{
public:
	bool				Create();
	bool				Destroy();
	bool				CreateView(int screenWidth, int screenHeight);
	bool				DestroyView();
	void				UpdateFrame(unsigned long frameTime);
	void				RenderFrame(int screenWidth, int screenHeight);

protected:
	void				RenderObjects(VShadingPass &shadePass, bool projectionPass);
	void				RenderDebug();
	bool				LoadShaders(VShadingPass &pass,
							const char *vertexFile1, const char *vertexFile2,
							const char *fragmFile1, const char *fragmFile2,
							GLint vertexIndex = 0, GLint normalIndex = -1, GLint texCoordIndex = -1, GLint colorIndex = -1);

	VGameState			mGameState;
	glm::mat4			mProjectionMatrix;
	glutil::MatrixStack	mModelViewStack;

	VPlaneVBO			mGround;
	VSphereVBO			mSphere;
	VLinesVBO			mAxis[3];
	VLinesVBO			mSpline;
	VTubeVBO			mTube;
	VShipVBO			mShip;
	VTrajectory			mTrajectory;

	VShadingPass		mFlatPass;
	VShadingPass		mTexPass;
	VTestTexture		mTexture;

	CPVRTPrint3D		mPrinter;
};

#endif