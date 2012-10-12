#include "VGlobal.h"

// from http://nehe.gamedev.net/data/articles/article.asp?article=08
void VUtil::GetLookAtFromDirection(const glm::vec3 &dir, glm::vec3 &fwd, glm::vec3 &up, glm::vec3 &right)
{
	fwd = dir;
	glm::normalize(fwd);

	glm::vec3 projectedDir = dir;

	const GLfloat epsilon = 0.00001f;

	if( fabs( dir[0] ) < epsilon && fabs( dir[2] ) < epsilon ) // YZ plane
	{
		projectedDir[0] = 0;
		glm::normalize(projectedDir);

		glm::vec3 tempRight( 1, 0, 0 );
		up = glm::cross(projectedDir, tempRight);

		right = glm::cross(fwd, up);
	}
	else // XZ plane
	{
		projectedDir[ 1 ] = 0;
		glm::normalize(projectedDir);

		glm::vec3 tempUp( 0, 1, 0 );
		right = -glm::cross(projectedDir, tempUp);

		up = glm::cross(fwd, right);
	}

	glm::normalize(right);
	glm::normalize(up);
}

void VUtil::SendMat4ToShader(const GLuint handle, const glm::mat4 &matrix, const char *varName)
{
	int varIndex = glGetUniformLocation(handle, varName);
	if( varIndex >= 0 )
		glUniformMatrix4fv(varIndex, 1, GL_FALSE, glm::value_ptr(matrix));
}

void VUtil::SendMat3ToShader(const GLuint handle, const glm::mat3 &matrix, const char *varName)
{
	int varIndex = glGetUniformLocation(handle, varName);
	if( varIndex >= 0 )
		glUniformMatrix3fv(varIndex, 1, GL_FALSE, glm::value_ptr(matrix));
}

void VUtil::SendVec4ToShader(const GLuint handle, const glm::vec4 &vec, const char *varName)
{
	int varIndex = glGetUniformLocation(handle, varName);
	if( varIndex >= 0 )
		glUniform4fv(varIndex, 1, glm::value_ptr(vec));
}