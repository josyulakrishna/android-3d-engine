#include "VGlobal.h"
#include "VVertexBufferObject.h"
#include <glm/gtx/spline.hpp>
#include <glm/gtx/rotate_vector.hpp>

bool VVertexBufferObject::Create()
{
	glGenBuffers(1, &mHandle);
	if( glGetError() != GL_NO_ERROR )
		return false;

	mStride = 0;
	mVertexCount = 0;

	return true;
}

bool VVertexBufferObject::Destroy()
{
	glDeleteBuffers(1, &mHandle);

	return true;
}

void VPlaneVBO::Fill()
{
	GLfloat afVertices[] = {
#define V 0.5f
		-V,  0, -V, // vertex
		0,  1,  0, // normal
		0,  0,		// tex coord - top left

		-V,  0, +V, // vertex
		0,  1,  0, // normal
		0,  1,		// tex coord

		+V,  0, +V, // vertex
		0,  1,  0, // normal
		1,  1,		// tex coord

		+V,  0, +V, // vertex
		0,  1,  0, // normal
		1,  1,		// tex coord

		+V,  0, -V, // vertex
		0,  1,  0, // normal
		1,  0,		// tex coord

		-V,  0, -V, // vertex
		0,  1,  0, // normal
		0,  0,		// tex coord
	};

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	mStride = (8 * sizeof(GLfloat));
	mVertexCount = 6;
	glBufferData(GL_ARRAY_BUFFER, mVertexCount * mStride, afVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VPlaneVBO::Draw(const VProgram &program)
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	glEnableVertexAttribArray(program.mVertexAttributeIndex);
	glVertexAttribPointer(program.mVertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(0));
	if( program.mNormalAttributeIndex > 0 ) {
		glEnableVertexAttribArray(program.mNormalAttributeIndex);
		glVertexAttribPointer(program.mNormalAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(3 * sizeof(GLfloat)));
	}
	if( program.mTexCoordAttributeIndex >= 0 ) {
		glEnableVertexAttribArray(program.mTexCoordAttributeIndex);
		glVertexAttribPointer(program.mTexCoordAttributeIndex, 2, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(6 * sizeof(GLfloat)));
	}

	glDrawArrays(GL_TRIANGLES, 0, mVertexCount);

	glDisableVertexAttribArray(program.mVertexAttributeIndex);
	if( program.mNormalAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mNormalAttributeIndex);
	}
	if( program.mTexCoordAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mTexCoordAttributeIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


inline void EmitVertex(GLfloat *ptrData, GLuint &index, GLfloat x, GLfloat y, GLfloat z)
{
	*(ptrData + index++) = x;
	*(ptrData + index++) = y;
	*(ptrData + index++) = z;
}

void drawPatch(GLfloat *ptrData, GLuint &ptrIndex, GLfloat p1[3], GLfloat p2[3], GLfloat p3[3], int level, GLuint *count)
{
	int i;
	if (level > 0) {
		GLfloat q1[3],q2[3],q3[3];		 // sub-vertices
		for (i=0; i<3; i++) {
			q1[i] = 0.5f*(p1[i]+p2[i]);
			q2[i] = 0.5f*(p2[i]+p3[i]);
			q3[i] = 0.5f*(p3[i]+p1[i]);
		}
		GLfloat length1 = (GLfloat)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
		GLfloat length2 = (GLfloat)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
		GLfloat length3 = (GLfloat)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
		for (i=0; i<3; i++) {
			q1[i] *= length1;
			q2[i] *= length2;
			q3[i] *= length3;
		}
		drawPatch(ptrData,ptrIndex,p1,q1,q3,level-1,count);
		drawPatch(ptrData,ptrIndex,q1,p2,q2,level-1,count);
		drawPatch(ptrData,ptrIndex,q1,q2,q3,level-1,count);
		drawPatch(ptrData,ptrIndex,q3,q2,p3,level-1,count);
	} else {
		if( count ) {
			*count += 3;
		} else {
			EmitVertex(ptrData,ptrIndex,p1[0],p1[1],p1[2]); // vertex
			EmitVertex(ptrData,ptrIndex,p1[0],p1[1],p1[2]); // normal
			EmitVertex(ptrData,ptrIndex,p2[0],p2[1],p2[2]); // vertex
			EmitVertex(ptrData,ptrIndex,p2[0],p2[1],p2[2]); // normal
			EmitVertex(ptrData,ptrIndex,p3[0],p3[1],p3[2]); // vertex
			EmitVertex(ptrData,ptrIndex,p3[0],p3[1],p3[2]); // normal
		}
	}
}

void drawSphere(GLfloat *ptrData, GLuint &ptrIndex, int sphere_quality, GLuint *count)
{
	// icosahedron data for an icosahedron of radius 1.0
#define ICX 0.525731112119133606f
#define ICZ 0.850650808352039932f
	static GLfloat idata[12][3] = {
		{-ICX, 0, ICZ},
		{ICX, 0, ICZ},
		{-ICX, 0, -ICZ},
		{ICX, 0, -ICZ},
		{0, ICZ, ICX},
		{0, ICZ, -ICX},
		{0, -ICZ, ICX},
		{0, -ICZ, -ICX},
		{ICZ, ICX, 0},
		{-ICZ, ICX, 0},
		{ICZ, -ICX, 0},
		{-ICZ, -ICX, 0}
	};

	static int index[20][3] = {
		{0, 4, 1},	  {0, 9, 4},
		{9, 5, 4},	  {4, 5, 8},
		{4, 8, 1},	  {8, 10, 1},
		{8, 3, 10},   {5, 3, 8},
		{5, 2, 3},	  {2, 7, 3},
		{7, 10, 3},   {7, 6, 10},
		{7, 11, 6},   {11, 0, 6},
		{0, 1, 6},	  {6, 1, 10},
		{9, 0, 11},   {9, 11, 2},
		{9, 2, 5},	  {7, 2, 11},
	};

	for (int i=0; i<20; i++) {
		drawPatch(ptrData, ptrIndex, &idata[index[i][2]][0],&idata[index[i][1]][0],
			&idata[index[i][0]][0],sphere_quality, count);
	}
}

void VSphereVBO::Fill()
{
	GLuint index = 0;
	drawSphere(NULL, index, mQuality, &mVertexCount);

	mStride = 6 * sizeof(GLfloat); // 3 floats for the pos, 3 for the normal
	int total = mVertexCount * mStride;
	index = 0;

	GLfloat *bufferData = new GLfloat[total];
	drawSphere(bufferData, index, mQuality, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, total, bufferData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete []bufferData;
}

void VSphereVBO::Draw(const VProgram &program)
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	glEnableVertexAttribArray(program.mVertexAttributeIndex);
	glVertexAttribPointer(program.mVertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, 0);
	if( program.mNormalAttributeIndex > 0 ) {
		glEnableVertexAttribArray(program.mNormalAttributeIndex);
		glVertexAttribPointer(program.mNormalAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(mStride / 2));
	}

	glDrawArrays(GL_TRIANGLES, 0, mVertexCount);

	glDisableVertexAttribArray(program.mVertexAttributeIndex);
	if( program.mNormalAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mNormalAttributeIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VLinesVBO::Fill()
{
	mStride = sizeof(glm::vec3); // 3 floats for the pos
	mVertexCount = mPoints.size();

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, mVertexCount * mStride, &mPoints[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VLinesVBO::Draw(const VProgram &program)
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	glEnableVertexAttribArray(program.mVertexAttributeIndex);
	glVertexAttribPointer(program.mVertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, 0);

	glDrawArrays(GL_LINE_STRIP, 0, mVertexCount);

	glDisableVertexAttribArray(program.mVertexAttributeIndex);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const float EPS = 0.001f;

bool VTubeVBO::Create()
{
	if( !VVertexBufferObject::Create() )
		return false;
	glGenBuffers(1, &mIndexHandle);
	if( glGetError() != GL_NO_ERROR )
		return false;

	return true;
}

bool VTubeVBO::Destroy()
{
	VVertexBufferObject::Destroy();
	glDeleteBuffers(1, &mIndexHandle);

	return true;
}

void VTubeVBO::Fill()
{
	VVertexList mSamplePoint;
	VIndexList mIndexList;

	mRadius = 1.f;
	mStride = sizeof(VVertexData);

	glm::vec3 dir = mCtrlPoints[1] - mCtrlPoints[0];
	glm::vec3 tangent0, normal0, binormal0;
	VUtil::GetLookAtFromDirection(dir, tangent0, normal0, binormal0);
	
	int vertexIndex = 0;
	const float stepInLength = 1.f / 15.f;
	const int stepInSection = 15; // degrees
	const int countPerSection = 360 / stepInSection;

	int u = 0;
	int v = 1;

	for( GLuint i = 0; i < mCtrlPoints.size() - 3; i++ ) {
		for( GLfloat t = 0.f; t < 1.f + EPS; t += stepInLength ) {
			const glm::vec3 &C0 = mCtrlPoints[i+0];
			const glm::vec3 &C1 = mCtrlPoints[i+1];
			const glm::vec3 &C2 = mCtrlPoints[i+2];
			const glm::vec3 &C3 = mCtrlPoints[i+3];
			glm::vec3 o = glm::catmullRom(C0, C1, C2, C3, t);

			const float diff = 0.05f;
			glm::vec3 tg0 = glm::catmullRom(C0, C1, C2, C3, glm::clamp(t - diff, 0.f, 1.f));
			glm::vec3 tg1 = glm::catmullRom(C0, C1, C2, C3, glm::clamp(t + diff, 0.f, 1.f));

			glm::vec3 tangent1 = glm::normalize(tg1 - tg0);
			glm::vec3 normal1 = glm::normalize(glm::cross(binormal0, tangent1));
			glm::vec3 binormal1 = glm::normalize(glm::cross(tangent1, normal1));

			for( int a = 0; a <= 360; a += stepInSection ) {
				glm::vec3 p = glm::rotate(normal1, float(a), tangent1);

				VVertexData vertex;
				vertex.mPosition = o + p * mRadius;
				vertex.mNormal = glm::normalize(p);
				vertex.mTexCoord = glm::vec2(u, v);
				mSamplePoint.push_back(vertex);

				// first tri
				mIndexList.push_back(vertexIndex);
				mIndexList.push_back(vertexIndex + 1);
				mIndexList.push_back(vertexIndex + countPerSection + 1);
				// second tri
				mIndexList.push_back(vertexIndex + countPerSection + 1);
				mIndexList.push_back(vertexIndex + 1);
				mIndexList.push_back(vertexIndex + countPerSection + 2);

				vertexIndex++;
				u = 1 - u;
			}

			tangent0 = tangent1;
			normal0 = normal1;
			binormal0 = binormal1;
			v = 1 - v;
			u = 0;
		}
	}
	mVertexCount = mSamplePoint.size();
	mIndexCount = mIndexList.size() - (countPerSection + 1) * 6;

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, mVertexCount * mStride, &mSamplePoint[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(GLushort), &mIndexList[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VTubeVBO::Draw(const VProgram &program)
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexHandle);

	glEnableVertexAttribArray(program.mVertexAttributeIndex);
	glVertexAttribPointer(program.mVertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(0));
	if( program.mNormalAttributeIndex > 0 ) {
		glEnableVertexAttribArray(program.mNormalAttributeIndex);
		glVertexAttribPointer(program.mNormalAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(3 * sizeof(GLfloat)));
	}
	if( program.mTexCoordAttributeIndex > 0 ) {
		glEnableVertexAttribArray(program.mTexCoordAttributeIndex);
		glVertexAttribPointer(program.mTexCoordAttributeIndex, 2, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(6 * sizeof(GLfloat)));
	}

	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(program.mVertexAttributeIndex);
	if( program.mNormalAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mNormalAttributeIndex);
	}
	if( program.mTexCoordAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mTexCoordAttributeIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VShipVBO::Fill()
{
	GLfloat afVertices[] = {
#undef V
#define V 0.25f
#define L 1.0f
#define N 0.707107f
		+V, -V, +0,
		+N, +N, +0,
		-V, +V, +0,
		+N, +N, +0,
		+0, +0, +L,
		+N, +N, +0,

		-V, -V, +0,
		-N, -N, +0,
		+V, +V, +0,
		-N, -N, +0,
		+0, +0, +L,
		-N, -N, +0,
	};

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	mStride = (6 * sizeof(GLfloat));
	mVertexCount = 6;
	glBufferData(GL_ARRAY_BUFFER, mVertexCount * mStride, afVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VShipVBO::Draw(const VProgram &program)
{
	int varIndex = glGetUniformLocation(program.mHandle, "in_Color");

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);

	glEnableVertexAttribArray(program.mVertexAttributeIndex);
	glVertexAttribPointer(program.mVertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(0));
	if( program.mNormalAttributeIndex > 0 ) {
		glEnableVertexAttribArray(program.mNormalAttributeIndex);
		glVertexAttribPointer(program.mNormalAttributeIndex, 3, GL_FLOAT, GL_FALSE, mStride, (const GLvoid *)(mStride / 2));
	}

	if( varIndex >= 0 )
		glUniform4f(varIndex, 1, 0.55f, 0.55f, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	if( varIndex >= 0 )
		glUniform4f(varIndex, 0.55f, 1, 0.55f, 0);
	glDrawArrays(GL_TRIANGLES, 3, 3);

	glDisableVertexAttribArray(program.mVertexAttributeIndex);
	if( program.mNormalAttributeIndex > 0 ) {
		glDisableVertexAttribArray(program.mNormalAttributeIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
