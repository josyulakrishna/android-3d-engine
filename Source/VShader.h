#ifndef __VSHADER
#define __VSHADER

#define VAR_IN_MODELVIEWMATRIX "in_ModelViewMatrix"
#define VAR_IN_PROJECTIONMATRIX "in_ProjectionMatrix"
#define VAR_IN_MODELVIEWPROJECTMATRIX "in_ModelViewProjectionMatrix"
#define VAR_IN_VERTEX	"in_Vertex"
#define VAR_IN_TEXCOORD	"in_TexCoord"
#define VAR_IN_NORMAL	"in_Normal"
#define VAR_IN_COLOR	"in_Color"
#define VAR_SAMPLER2D	"in_TexUnit2D"

#include <vector>

typedef std::vector<VString> VFileList;

class VShader
{
public:
	bool				Create(GLuint type, const VString &data, VString *logMessage = 0);
	bool				Destroy();

	GLuint				mHandle;
	GLuint				mType;
};

class VProgram
{
public:
	/* constructor */	VProgram();
	bool				Create(const VShader &vertexShader, const VShader &fragmShader, VString *logMessage = 0);
	bool				Destroy();
	void				SetupVariables(const glm::mat4 &projectionMatrix, const glm::mat4 &cameraMatrix, const GLuint tex2DUnit);
	void				SetupVertexAttributes(GLint vertexIndex = 0, GLint normalIndex = -1, GLint texCoordIndex = -1, GLint colorIndex = -1);

	GLuint				mHandle;
	GLint				mVertexAttributeIndex;
	GLint				mTexCoordAttributeIndex;
	GLint				mNormalAttributeIndex;
	GLint				mColorAttributeIndex;
};

class VShadingPass
{
public:
	bool				Compile(const VFileList &vertexShaders, const VFileList &fragmShaders);
	bool				Link();
	void				Use() const;
	bool				Destroy();

	VShader				mVertexShader;
	VShader				mFragmentShader;
	VProgram			mProgram;
	VString				mErrorLog;

protected:
	bool				CompileShader(const char *fileName, VShader &src, GLuint type);
};

inline VProgram::VProgram()
{
	mHandle = 0;
	mVertexAttributeIndex = 0;
	mNormalAttributeIndex = -1;
	mTexCoordAttributeIndex = -1;
	mColorAttributeIndex = -1;
}

#endif