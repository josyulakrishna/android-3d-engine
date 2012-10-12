#include "VGlobal.h"
#include "VApp.h"
#include <glutil/MousePoles.h>

const int TEX_UNIT_OTHERS = 0;
const int TEX_UNIT_SHADOW = 1;

glutil::ViewData g_initialViewData = {
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::fquat(glm::vec3(0, PI/3, 0)),
	10.0f,
	0.0f
};

glutil::ViewScale g_viewScale =
{
	0.0f, 50.0f,
	5.5f, 0.2f,
	0.0f, 0.0f,	// No camera movement.
	90.0f/250.0f
};

glutil::ViewPole g_viewPole = glutil::ViewPole(
	g_initialViewData,
	g_viewScale, 
	glutil::MB_LEFT_BTN
);

void ForwardMouseMotion(int x, int y)
{
    g_viewPole.MouseMove(glm::ivec2(x, y));
}

void ForwardMouseButton(int button, bool pressed, int x, int y)
{
	g_viewPole.MouseClick(glutil::MB_LEFT_BTN, pressed, 0, glm::ivec2(x, y));
}

void ForwardMouseWheel(int direction, int x, int y)
{
	g_viewPole.MouseWheel(direction, 0, glm::ivec2(x, y));
}

bool VApp::Create()
{
	return true;
}

bool VApp::Destroy()
{
	return true;
}

bool VApp::LoadShaders(
	VShadingPass &pass,
	const char *vertexFile1, const char *vertexFile2,
	const char *fragmFile1, const char *fragmFile2,
	GLint vertexIndex, GLint normalIndex, GLint texCoordIndex, GLint colorIndex)
{
	VFileList vertexList;
	vertexList.push_back(vertexFile1);
	if( vertexFile2 ) {
		vertexList.push_back(vertexFile2);
	}
	VFileList fragmList;
	fragmList.push_back(fragmFile1);
	if( fragmFile2 ) {
		fragmList.push_back(fragmFile2);
	}

	if( !pass.Compile(vertexList, fragmList) ) {
		VUtil::Log(pass.mErrorLog.c_str());
		return false;
	}
	pass.mProgram.SetupVertexAttributes(vertexIndex, normalIndex, texCoordIndex, colorIndex);
	if( !pass.Link() ) {
		VUtil::Log(pass.mErrorLog.c_str());
		return false;
	}

	return true;
}

void AddRandomPointToSpline(VPointList &list)
{
	glm::vec3 &prev = list.back();

	GLfloat r = GLfloat(rand()) / GLfloat(RAND_MAX);
	GLfloat theta = r * (PI*2);

	r = GLfloat(rand()) / GLfloat(RAND_MAX);
	GLfloat phi = r * (PI);
	 
	const GLfloat R = 7;

	glm::vec3 point(
		prev.x + R * cos(theta) * sin(phi),
		prev.y + R * sin(theta) * sin(phi),
		prev.z + R * cos(phi)
	);

	list.push_back(point);
}

void AddRandomPointToSpline2(VPointList &list)
{
	int N = list.size();
	glm::vec3 dir = list[N-1] - list[N-2];

	glm::vec3 up, fwd, right;
	VUtil::GetLookAtFromDirection(dir, fwd, up, right);
	glm::mat3 base = glm::mat3(1);
	base[0] = fwd;
	base[1] = up;
	base[2] = right;

again:
	GLfloat r = GLfloat(rand()) / GLfloat(RAND_MAX);
	GLfloat theta = r * (PI/2);

	r = GLfloat(rand()) / GLfloat(RAND_MAX);
	GLfloat phi = r * (PI/4);
	 
	const GLfloat R = 7;

	glm::vec3 point(
		cos(theta) * sin(phi),
		sin(theta) * sin(phi),
		cos(phi)
	);
	glm::vec3 p = glm::normalize(base * point);
	if( glm::dot(p, glm::normalize(list[N-1])) < 0 )
		goto again;

	list.push_back(list[N-1] + p * R);
}

bool VApp::CreateView(int screenWidth, int screenHeight)
{
	srand(1667);

	const GLfloat N = 10;
	mAxis[0].mPoints.push_back(glm::vec3(0, 0, 0));
	mAxis[0].mPoints.push_back(glm::vec3(N, 0, 0));
	mAxis[0].Create();
	mAxis[0].Fill();
	mAxis[1].mPoints.push_back(glm::vec3(0, 0, 0));
	mAxis[1].mPoints.push_back(glm::vec3(0, N, 0));
	mAxis[1].Create();
	mAxis[1].Fill();
	mAxis[2].mPoints.push_back(glm::vec3(0, 0, 0));
	mAxis[2].mPoints.push_back(glm::vec3(0, 0, N));
	mAxis[2].Create();
	mAxis[2].Fill();

	mGround.Create();
	mGround.Fill();
	mSphere.mQuality = 1;
	mSphere.Create();
	mSphere.Fill();

	mSpline.mPoints.push_back(glm::vec3(0, 0, 0));
	mSpline.mPoints.push_back(glm::vec3(1, 1, 1));
	for( int i = 0; i < 5 + 2; i++ ) {
		AddRandomPointToSpline2(mSpline.mPoints);
	}
	//mSpline.mPoints.push_back(glm::vec3(-2, 0, 6));
	//mSpline.mPoints.push_back(glm::vec3(4, 0, 5));
	//mSpline.mPoints.push_back(glm::vec3(4, 0, 1));
	//mSpline.mPoints.push_back(glm::vec3(-2, 0, -2));
	//mSpline.mPoints.push_back(glm::vec3(-2, 0, -4));
	mSpline.Create();
	mSpline.Fill();

	mTube.mCtrlPoints = mSpline.mPoints;
	mTube.Create();
	mTube.Fill();

	mShip.Create();
	mShip.Fill();

	if( !LoadShaders(mFlatPass, "DirectVertex.vert", NULL, "DirectColor.frag", NULL) )
		return false;
	if( !LoadShaders(mTexPass, "Texture.vert", NULL, "Texture.frag", NULL, 0, 1, 2) )
		return false;

	mTexture.Create();
	mPrinter.SetTextures(NULL, screenWidth, screenHeight);

	mTrajectory.Setup(&mSpline.mPoints);

	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0, 1);

	glDisable(GL_DITHER);
//	glDisable(GL_ALPHA_TEST); // no more in ES 2.0
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_SCISSOR_TEST);

	return true;
}

bool VApp::DestroyView()
{
	mAxis[0].Destroy();
	mAxis[1].Destroy();
	mAxis[2].Destroy();

	mGround.Destroy();
	mSphere.Destroy();

	mSpline.Destroy();
	mTube.Destroy();
	mShip.Destroy();

	mFlatPass.Destroy();
	mTexPass.Destroy();

	mTexture.Destroy();
	mPrinter.ReleaseTextures();

	return true;
}

static unsigned long prevTime = 0;
static float dt;

void VApp::UpdateFrame(unsigned long frameTime)
{
	mTrajectory.Advance(0.005f);

	if( prevTime == 0 )
		prevTime = frameTime;		
	dt = float(frameTime - prevTime) * 0.001f;
	prevTime = frameTime;
}

void VApp::RenderFrame(int screenWidth, int screenHeight)
{
	GLfloat aspectRatio = GLfloat(screenWidth) / GLfloat(screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
	mProjectionMatrix = glm::perspective(60.0f, aspectRatio, 0.2f, 50.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	mModelViewStack.SetMatrix(g_viewPole.CalcMatrix());
	mModelViewStack.SetMatrix(mTrajectory.mCamOrient);

	RenderObjects(mTexPass, false);

	RenderDebug();

	char str[16];
	sprintf(str, "%2.1f\0", (1.f / dt));
	mPrinter.DisplayDefaultTitle(str, NULL, 0);
	mPrinter.Flush();
}

void VApp::RenderObjects(VShadingPass &pass, bool projectionPass)
{
	pass.Use();

	glActiveTexture(GL_TEXTURE0 + TEX_UNIT_OTHERS);
	glBindTexture(GL_TEXTURE_2D, mTexture.mHandle);

	{ // draw the track
		glutil::PushStack push(mModelViewStack);

		pass.mProgram.SetupVariables(mProjectionMatrix, mModelViewStack.Top(), TEX_UNIT_OTHERS);
		mTube.Draw(pass.mProgram);
	}

	//{ // draw the ship
	//	glutil::PushStack push(mModelViewStack);
	//	mModelViewStack.ApplyMatrix(mTrajectory.mLocalOrient);
	//	mModelViewStack.RotateY(80);
	//	mModelViewStack.Scale(0.25f);

	//	pass.mProgram.SetupVariables(mProjectionMatrix, mModelViewStack.Top(), TEX_UNIT_OTHERS);

	//	glDisable(GL_CULL_FACE);
	//	mShip.Draw(pass.mProgram);
	//	glEnable(GL_CULL_FACE);
	//}
}

void VApp::RenderDebug()
{
	mFlatPass.Use();

	{ // draw axes
		glutil::PushStack push(mModelViewStack);

		mFlatPass.mProgram.SetupVariables(mProjectionMatrix, mModelViewStack.Top(), 0);
		VUtil::SendVec4ToShader(mFlatPass.mProgram.mHandle, glm::vec4(1, 0, 0, 0), "in_Color");
		mAxis[0].Draw(mFlatPass.mProgram);
		VUtil::SendVec4ToShader(mFlatPass.mProgram.mHandle, glm::vec4(0, 1, 0, 0), "in_Color");
		mAxis[1].Draw(mFlatPass.mProgram);
		VUtil::SendVec4ToShader(mFlatPass.mProgram.mHandle, glm::vec4(0, 0, 1, 0), "in_Color");
		mAxis[2].Draw(mFlatPass.mProgram);
	}

	{ // draw the spline segments
		glutil::PushStack push(mModelViewStack);
		mModelViewStack.Translate(0, 1.5f, 0);

		mFlatPass.mProgram.SetupVariables(mProjectionMatrix, mModelViewStack.Top(), 0);
		VUtil::SendVec4ToShader(mFlatPass.mProgram.mHandle, glm::vec4(1, 1, 0, 0), "in_Color");

		mSpline.Draw(mFlatPass.mProgram);
	}
}
