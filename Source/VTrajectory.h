#ifndef __VTRAJECTORY
#define __VTRAJECTORY

typedef std::vector<glm::vec3> VPointList;

class VTrajectory
{
public:
	void				Setup(const VPointList *spline);
	void				Advance(GLfloat tIncr);

	glm::vec3			mCamPos;
	glm::vec3			mCamTarget;
	glm::mat4			mCamOrient;
	GLfloat				mAngle;
	GLfloat				mPosRadius;
	GLfloat				mTargetRadius;

	glm::vec3			mTangent;
	glm::vec3			mNormal;
	glm::vec3			mBinormal;
	VPointList			mSplineCtrlPoint;
	GLuint				mSegmentIndex;
	GLfloat				t;
	glm::mat4			mLocalOrient; // matrix used to xform world-space objects to the local camera-on-tube space
};

#endif