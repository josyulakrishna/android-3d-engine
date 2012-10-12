#include "VGlobal.h"
#include "VShell.h"

bool VShell::InitApplication()
{
	VString path = (char*)PVRShellGet(prefReadPath);
#ifdef WIN32
	path += "..\\..\\..\\Android\\assets\\";
#endif
	CPVRTResourceFile::SetReadPath(path.c_str());
	CPVRTResourceFile::SetLoadReleaseFunctions(PVRShellGet(prefLoadFileFunc), PVRShellGet(prefReleaseFileFunc));

	PVRShellSet(prefColorBPP, 32);
	//PVRShellSet(prefOutputInfo, GL_TRUE);

	return mApp.Create();
}

VShell *gShell = NULL;

PVRShell* NewDemo()
{
	gShell = new VShell();
	return gShell;
}


void VUtil::Log(const char *str)
{
	if( gShell )
		gShell->PVRShellSet(prefExitMessage, str);
}