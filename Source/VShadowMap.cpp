#include "VGlobal.h"
#include "VShadowMap.h"

VResult VShadowMap::Create(const GLuint texUnit)
{
	if( !mDepthTexture.Create() )
		return VResult(false);

	mTexUnitIndex = texUnit;

	// get and save the orignal framebuffer object - the one linked to the viewing window
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mPrimaryFBO);

	// generate an auxiliary render buffer object
	//glGenRenderbuffers(1, &mSecondaryRBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, mSecondaryRBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mShadowMap.mSize[0], mShadowMap.mSize[1]);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// generate an auxiliary framebuffer to which either textures or render-objects can be attached
	glGenFramebuffers(1, &mSecondaryFBO);
	if( glGetError() != GL_NO_ERROR ) {
		return VResult(false);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, mSecondaryFBO);

	// attach a texture to the framebuffer to draw into
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture.mHandle, 0);

	// attach a depth render object to this framebuffer
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mSecondaryRBO);

	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		return VResult(false, VString("ERROR: Frame buffer not set up correctly\n"));
	}

	// revert
	glBindFramebuffer(GL_FRAMEBUFFER, mPrimaryFBO);

	return VResult(true);
}

VResult VShadowMap::Destroy()
{
	glDeleteFramebuffers(1, &mSecondaryFBO);
	mDepthTexture.Destroy();

	return VResult(true);
}

bool VShadowMap::BeginRenderPass(GLfloat fov, GLfloat zNear, GLfloat zFar)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mSecondaryFBO);
	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		glBindFramebuffer(GL_FRAMEBUFFER, mPrimaryFBO);
		return false;
	}

	// setup the drawing viewport to match the texture
	glViewport(0, 0, mDepthTexture.mSize[0], mDepthTexture.mSize[1]);

	// careful when selecting the zNear/zFar planes as can drastically affect
	// the accuracy of the shadow map
	mProjectionMatrix = glm::perspective(fov, 1.f, zNear, zFar);

	glClear(GL_DEPTH_BUFFER_BIT);

	// Since we don't care about colour when rendering the depth values to
	// the shadow-map texture, we disable color writing to increase speed.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	{
		glm::vec3 lightCamUp;
		glm::vec3 lightCamFwd;
		glm::vec3 lightCamRight;
		glm::vec3 lightDir = mLightPos - mLightTarget;
		VUtil::GetLookAtFromDirection(lightDir, lightCamFwd, lightCamUp, lightCamRight);
		mModelViewMatrix = glm::lookAt(mLightPos, mLightTarget, lightCamUp);
		//glm::mat4 Translate = glm::mat4(1);
		//Translate[3] = glm::vec4(-mLightPos, 1.f);
		//lightMVMatrix = LookAt * Translate;
	}

	{
		// compute in reverse order the tranformations needed to go from
		// a point in camera(eye) space to the clip-space of the light virtual view
		// and then to the [0, 1] texture coordinate space
		mMVPStack.SetIdentity();

		// 3) transform from clip-space [-1, 1] to [0, 1]
		mMVPStack.Translate(0.5f, 0.5f, 0.0f);
		mMVPStack.Scale(0.5f, 0.5f, 1.0f);

		// 2) go from world to light's clip-space (basically rasterize from the light's POV)
		mMVPStack.ApplyMatrix(mProjectionMatrix * mModelViewMatrix);

		// 1) append other transformations
		// this will be handled in the second pass (the visual render)
	}

	return true;
}

void VShadowMap::EndRenderPass()
{
	// revert to full color write
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, mPrimaryFBO);
}

void VShadowMap::SetupVariables(const VProgram &program, const glm::mat4 &modelViewMatrix)
{
	{ // setup shadow map
		glActiveTexture(GL_TEXTURE0 + mTexUnitIndex);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture.mHandle);

		int texUnitIndex = glGetUniformLocation(program.mHandle, VAR_IN_SHADOWSAMPLER2D);
		if( texUnitIndex >= 0 )
			glUniform1i(texUnitIndex, mTexUnitIndex);
	}

	// complete the transform that goes from normal camera space into the light's eye space
	mMVPStack.ApplyMatrix(glm::inverse(modelViewMatrix));
	VUtil::SendMat4ToShader(program.mHandle, mMVPStack.Top(), VAR_IN_LIGHTMVPMATRIX);

#ifdef LIGHTING
	// transform the light direction in camera space and pass it to the shader
	glm::vec3 lightDir = mLightPos - mLightTarget;
	glm::vec4 lightDirInCamera = glm::normalize(modelViewMatrix * glm::vec4(lightDir, 0));
	int lightPosVarIndex = glGetUniformLocation(program.mHandle, VAR_IN_LIGHTDIR);
	if( lightPosVarIndex >= 0 )
		glUniform3fv(lightPosVarIndex, 1, glm::value_ptr(lightDirInCamera));

	// compute the matrix that transforms (model) normals into camera space
	// it's the inverse of the camera matrix without the translation (normals are only directions)
	// then pass to the vertex shader
	glm::mat3 normalsToCamera(modelViewMatrix);
	normalsToCamera = glm::transpose(glm::inverse(normalsToCamera));
	VUtil::SendMat3ToShader(program.mHandle, normalsToCamera, VAR_IN_NORMALS2VIEWMATRIX);
#endif
}