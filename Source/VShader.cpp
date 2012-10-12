#include "VGlobal.h"
#include "VShader.h"

VString GetShaderBuildInfo(GLuint shaderHandle)
{
	GLint textLength = 0;
	glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &textLength);

	const GLint maxTextLength = 1024;
	GLchar textBuffer[maxTextLength] = { 0 };

	GLsizei charsWritten = 0;
	glGetShaderInfoLog(shaderHandle, maxTextLength, &charsWritten, textBuffer);

	return VString(textBuffer);
}

VString GetProgramBuildInfo(GLuint programHandle)
{
	GLint textLength = 0;
	glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &textLength);

	const GLint maxTextLength = 1024;
	GLchar textBuffer[maxTextLength] = { 0 };

	GLsizei charsWritten = 0;
	glGetProgramInfoLog(programHandle, maxTextLength, &charsWritten, textBuffer);

	return VString(textBuffer);
}

bool VShader::Create(GLuint type, const VString &data, VString *logMessage)
{
	mHandle = glCreateShader(type);
	if( glGetError() != GL_NO_ERROR )
		return false;
	
	const GLchar *src[] = { data.c_str() };
	glShaderSource(mHandle, 1, src, NULL);

	glCompileShader(mHandle);

	GLint compilationResult = GL_FALSE;
	glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compilationResult);
	if( logMessage )
		*logMessage = GetShaderBuildInfo(mHandle);
	if( !compilationResult )
		return false;

	mType = type;

	return true;
}

bool VShader::Destroy()
{
	glDeleteShader(mHandle);

	return true;
}

void VProgram::SetupVertexAttributes(GLint vertexIndex, GLint normalIndex, GLint texCoordIndex, GLint colorIndex)
{
	mVertexAttributeIndex = vertexIndex;
	mNormalAttributeIndex = normalIndex;
	mTexCoordAttributeIndex = texCoordIndex;
	mColorAttributeIndex = colorIndex;
}

bool VProgram::Create(const VShader &vertexShader, const VShader &fragmShader, VString *logMessage)
{
	mHandle = glCreateProgram();
	if( glGetError() != GL_NO_ERROR )
		return false;

	if( mVertexAttributeIndex < 0 )
		return false;
	glBindAttribLocation(mHandle, mVertexAttributeIndex, VAR_IN_VERTEX);
	if( mNormalAttributeIndex >= 0 )
		glBindAttribLocation(mHandle, mNormalAttributeIndex, VAR_IN_NORMAL);
	if( mTexCoordAttributeIndex >= 0 )
		glBindAttribLocation(mHandle, mTexCoordAttributeIndex, VAR_IN_TEXCOORD);
	if( mColorAttributeIndex >= 0 )
		glBindAttribLocation(mHandle, mColorAttributeIndex, VAR_IN_COLOR);

	glAttachShader(mHandle, vertexShader.mHandle);
	glAttachShader(mHandle, fragmShader.mHandle);

	glLinkProgram(mHandle);

	//TODO: we can detach and destroy the shader objects here

	GLint linkingResult = GL_FALSE;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &linkingResult);
	if( !linkingResult ) {
		if( logMessage )
			*logMessage = GetProgramBuildInfo(mHandle);

		return false;
	}

	return true;
}

void VProgram::SetupVariables(const glm::mat4 &projectionMatrix, const glm::mat4 &cameraMatrix, const GLuint texUnit)
{
// Sets the sampler variable for the 2D texture unit
	int texUnitIndex = glGetUniformLocation(mHandle, VAR_SAMPLER2D);
	if( texUnitIndex >= 0 )
		glUniform1i(texUnitIndex, texUnit);

// setup matrices
	int varIndex = glGetUniformLocation(mHandle, VAR_IN_MODELVIEWPROJECTMATRIX);
	if( varIndex >= 0 )
		glUniformMatrix4fv(varIndex, 1, GL_FALSE, glm::value_ptr(projectionMatrix * cameraMatrix));

	varIndex = glGetUniformLocation(mHandle, VAR_IN_MODELVIEWMATRIX);
	if( varIndex >= 0 )
		glUniformMatrix4fv(varIndex, 1, GL_FALSE, glm::value_ptr(cameraMatrix));

	varIndex = glGetUniformLocation(mHandle, VAR_IN_PROJECTIONMATRIX);
	if( varIndex >= 0 )
		glUniformMatrix4fv(varIndex, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

bool VProgram::Destroy()
{
	glDeleteProgram(mHandle);

	return true;
}

bool VShadingPass::CompileShader(const char *fileName, VShader &src, GLuint type)
{
	CPVRTResourceFile shaderFile(fileName);
	if( !shaderFile.IsOpen() )
		return false;

	VString log;
	VString bigString((const char*) shaderFile.DataPtr(), shaderFile.Size());
	if( !src.Create(type, bigString.c_str(), &log ) ) {
		mErrorLog = "Error in ";
		mErrorLog += fileName;
		mErrorLog += ":\n";
		mErrorLog += log;

		return false;
	}

	return true;
}

bool VShadingPass::Compile(const VFileList &vertexShaders, const VFileList &fragmShaders)
{
	VString bigSourceStr;

	for( VFileList::const_iterator i = vertexShaders.begin(); i != vertexShaders.end(); ++i ) {
		const VString &fileName = *i;
		CPVRTResourceFile shaderFile(fileName.c_str());
		if( !shaderFile.IsOpen() )
			return false;

		VString fileContent((const char*) shaderFile.DataPtr(), shaderFile.Size());
		bigSourceStr += fileContent;
	}

	if( !mVertexShader.Create(GL_VERTEX_SHADER, bigSourceStr.c_str(), &mErrorLog ) ) {
		return false;
	}

	bigSourceStr.clear();

	for( VFileList::const_iterator i = fragmShaders.begin(); i != fragmShaders.end(); ++i ) {
		const VString &fileName = *i;
		CPVRTResourceFile shaderFile(fileName.c_str());
		if( !shaderFile.IsOpen() )
			return false;

		VString fileContent((const char*) shaderFile.DataPtr(), shaderFile.Size());
		bigSourceStr += fileContent;
	}

	if( !mFragmentShader.Create(GL_FRAGMENT_SHADER, bigSourceStr.c_str(), &mErrorLog ) ) {
		return false;
	}

	return true;
}

bool VShadingPass::Link()
{
	if( !mProgram.Create(mVertexShader, mFragmentShader, &mErrorLog) )
		return false;

	return true;
}

void VShadingPass::Use() const
{
	glUseProgram(mProgram.mHandle);
}

bool VShadingPass::Destroy()
{
	mVertexShader.Destroy();
	mFragmentShader.Destroy();

	mProgram.Destroy();

	return true;
}
