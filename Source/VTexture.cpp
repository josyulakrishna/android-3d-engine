#include "VGlobal.h"
#include "VTexture.h"

bool VTexture::Create()
{
	glGenTextures(1, &mHandle);
	if( glGetError() != GL_NO_ERROR )
		return false;

	mSize[0] = mSize[1] = 0;

	return true;
}

bool VTexture::Destroy()
{
	glDeleteTextures(1, &mHandle);

	return true;
}

bool VTestTexture::Create()
{
	if( !VTexture::Create() )
		return false;

	const int SIZE = 64;
	mSize[0] = mSize[1] = SIZE;

	glBindTexture(GL_TEXTURE_2D, mHandle);

	GLuint pTexData[SIZE * SIZE];

	// Fills the data with a fancy pattern
	//for( int i = 0; i < SIZE; i++ )
	//	for( int j = 0; j < SIZE; j++ ) {		
	//		GLuint col = (255L<<24) + ((255L-j*2)<<16) + ((255L-i)<<8) + (255L-i*2);
	//		if ( ((i*j)/8) % 2 )
	//			col = (GLuint) (255L<<24) + (255L<<16) + (0L<<8) + (255L);
	//		pTexData[j*SIZE+i] = col;
	//	}

	// fill everything with white
	memset(&pTexData, 0xFFFFFFFF, SIZE * SIZE * sizeof(GLuint));
	// have borders black
	for( int y = 0; y < SIZE/20; y++ ) {
		for( int x = 0; x < SIZE; x++ ) {
			pTexData[ y * SIZE + x] = 0;
			pTexData[ (SIZE-y-1) * SIZE + x] = 0x00000000;
		}
	}
	for( int x = 0; x < SIZE/20; x++ ) {
		for( int y = 0; y < SIZE; y++ ) {		
			pTexData[ y * SIZE + x] = 0;
			pTexData[ y * SIZE + (SIZE-x-1)] = 0x00000000;
		}
	}


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE, SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexData);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}


bool VDepthTexture::Create()
{
	if( !VTexture::Create() )
		return false;

	const int SIZE = 512;
	mSize[0] = mSize[1] = SIZE;

	glBindTexture(GL_TEXTURE_2D, mHandle);

	// Each element is a single depth value.
	// The GL converts it to floating point, multiplies by the signed scale factor
	// GL_DEPTH_SCALE, adds the signed bias GL_DEPTH_BIAS,
	// and clamps to the range [0,1] 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SIZE, SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool VPVRTexture::Create()
{
	mHandle = 0;

	return false;
}

VResult VPVRTexture::Create(const char *fileName)
{
	if( PVRTTextureLoadFromPVR(fileName, &mHandle) != PVR_SUCCESS )
		return VResult(false, VString("ERROR: Couldn't load ") + fileName);

	return VResult(true);
}