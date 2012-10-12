#include "VGlobal.h"
#include "VTrajectory.h"
#include "VShader.h"
#include "VVertexBufferObject.h"
#include <glm/gtx/spline.hpp>
#include <glm/gtx/rotate_vector.hpp>

void VTrajectory::Setup(const VPointList *spline)
{
	if( spline )
		mSplineCtrlPoint = *spline;

// calculate the starting Frenet frame
	glm::vec3 dir = mSplineCtrlPoint[0] - mSplineCtrlPoint[1];
	VUtil::GetLookAtFromDirection(dir, mTangent, mNormal, mBinormal);

// reset the various variables
	mSegmentIndex = 0;
	t = 0;
	mPosRadius = -1.3f;
	mTargetRadius = -1.2f;
	mAngle = 0;
}

void Interpolate(glm::vec3 &out, const VPointList &list, const GLuint index, const GLfloat t0)
{
	GLuint i = index;
	GLfloat t = t0 + 0.2f;
	if( t > 1.f ) {
		t = t - 1.f;
		i++;
		if( i == list.size() - 3 ) {
			t = 1;
			i--;
		}
	}

	const glm::vec3 &C0 = list[i+0];
	const glm::vec3 &C1 = list[i+1];
	const glm::vec3 &C2 = list[i+2];
	const glm::vec3 &C3 = list[i+3];
	out = glm::catmullRom(C0, C1, C2, C3, t);
}

void VTrajectory::Advance(GLfloat tIncr)
{
// interpolate a point on the spline at a particular (local) t that varies from [0, 1]
	const glm::vec3 &C0 = mSplineCtrlPoint[mSegmentIndex+0];
	const glm::vec3 &C1 = mSplineCtrlPoint[mSegmentIndex+1];
	const glm::vec3 &C2 = mSplineCtrlPoint[mSegmentIndex+2];
	const glm::vec3 &C3 = mSplineCtrlPoint[mSegmentIndex+3];
	glm::vec3 o = glm::catmullRom(C0, C1, C2, C3, t);

// calculate the tangent from the diff of 2 interpolated points at close distances
	const float diff = 0.05f;
	glm::vec3 tg0 = glm::catmullRom(C0, C1, C2, C3, glm::clamp(t - diff, 0.f, 1.f));
	glm::vec3 tg1 = glm::catmullRom(C0, C1, C2, C3, glm::clamp(t + diff, 0.f, 1.f));

// get the Frenet frame for this position
// an iterative method is used where a starting Frenet frame is given and then advanced
	mTangent = glm::normalize(tg1 - tg0);
	mNormal = glm::normalize(glm::cross(mBinormal, mTangent));
	mBinormal = glm::normalize(glm::cross(mTangent, mNormal));

// rotate the camera position around the tangent axis
	glm::vec3 n = glm::rotate(mNormal, mAngle, mTangent);
	mCamPos = o + n * mPosRadius;

// get the camera position and rotate it as well around the tangent
	glm::vec3 o2;
	Interpolate(o2, mSplineCtrlPoint, mSegmentIndex, t);
	mCamTarget = o2 + n * mTargetRadius;

// the lookAt function calculates a transformation matrix for the camera composed of:
// 1) an orientation based on forward/up/right vectors
// 2) a translation of -(camera position)
	mCamOrient = glm::lookAt(mCamPos, mCamTarget, -n);

// compute the local space into-transform matrix
// matrix is addresed column-major
	mLocalOrient = glm::mat4(1); // make it identity
	mLocalOrient[0] = glm::vec4(mTangent, 0);
	mLocalOrient[1] = glm::vec4(mNormal, 0);
	mLocalOrient[2] = glm::vec4(mBinormal, 0);
	mLocalOrient[3] = glm::vec4((mCamTarget + mCamPos)/2.f, 1);

// increment (local) t and reset it at the next segment spline boundary
// at the end of the segments reset everything
	t += tIncr;
	if( t > 1.f ) {
		mSegmentIndex++;
		if( mSegmentIndex == mSplineCtrlPoint.size() - 3 ) {
			Setup(NULL);
			return;
		}
		t = 0;
	}

// increment the rotation angle and wrap it
	mAngle += 1.0f;
	if( mAngle >= 360 ) {
		mAngle = 0;
	}
}