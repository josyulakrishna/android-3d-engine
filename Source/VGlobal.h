#ifndef __VGLOBAL
#define __VGLOBAL

#include "PVRShell.h"
#include "OGLES2Tools.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glutil/MatrixStack.h>
#define PI 3.14159265358979323846264338327950288f

typedef CPVRTString VString;

struct VResult
{
	/* constructor */	VResult() { ok = false; }
	inline explicit		VResult(bool val) { ok = val; }
	inline explicit		VResult(bool val, const char *str) { ok = val; msg = str; }
	inline explicit		VResult(bool val, const VString &str) { ok = val; msg = str; }

	// prety unsafe - use knowingly
	// see http://www.artima.com/cppsource/safebool.html
	inline				operator bool() { return ok; }

	bool				ok;
	VString				msg;
};

struct VUtil
{
	static void			Log(const char *);
	static void			GetLookAtFromDirection(const glm::vec3 &dir, glm::vec3 &fwd, glm::vec3 &up, glm::vec3 &right);
	static void			SendMat4ToShader(const GLuint handle, const glm::mat4 &matrix, const char *varName);
	static void			SendMat3ToShader(const GLuint handle, const glm::mat3 &matrix, const char *varName);
	static void			SendVec4ToShader(const GLuint handle, const glm::vec4 &vec, const char *varName);
};

#endif