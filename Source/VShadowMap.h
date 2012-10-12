#ifndef __VSHADOWMAP
#define __VSHADOWMAP

#define VAR_IN_LIGHTDIR "in_LightDir"
#define VAR_IN_SHADOWSAMPLER2D "in_ShadowTexUnit"
#define VAR_IN_NORMALS2VIEWMATRIX "in_NormalsToViewMatrix"
#define VAR_IN_LIGHTMVPMATRIX "in_LightMVPMatrix"

#include "VTexture.h"
#include "VShader.h"

class VShadowMap
{
public:
	VResult				Create(const GLuint texUnit);
	VResult				Destroy();
	bool				BeginRenderPass(GLfloat fov, GLfloat zNear, GLfloat zFar);
	void				EndRenderPass();
	void				SetupVariables(const VProgram &program, const glm::mat4 &modelViewMatrix);

	GLint				mPrimaryFBO; // the main frame buffer object
	GLuint				mSecondaryFBO; // another frame buffer into which will draw
	GLuint				mSecondaryRBO; // a render-buffer-object that gets attached to the secondary frame-buffer-object (UNUSED for now)
	VDepthTexture		mDepthTexture; // depth of the scene from the lights pov gets written into this
	GLuint				mTexUnitIndex; // the texture stage where the shadow map texture gets bound

	glm::vec3			mLightPos; // light spotlight position (cone end)
	glm::vec3			mLightTarget; // light spotlight target

	glm::mat4			mProjectionMatrix;
	glm::mat4			mModelViewMatrix;
	// this will be used in 2 ways:
	// A) on the first render pass it will function as a regular MVP matrix: will transform world space to the lights eye/camera
	// B) on the second render pass it will back-project vertices from normal eye space into the light's eye space
	glutil::MatrixStack mMVPStack;
};

#endif