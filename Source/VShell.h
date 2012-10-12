#ifndef __VSHELL
#define __VSHELL

#include "VApp.h"

class VShell : public PVRShell
{
public:
	virtual bool		InitApplication();
	virtual bool		QuitApplication();
	virtual bool		InitView();
	virtual bool		ReleaseView();
	virtual bool		RenderScene();

	VApp				mApp;
};

inline bool VShell::QuitApplication()
{
	return mApp.Destroy();
}

inline bool VShell::InitView()
{
	return mApp.CreateView(PVRShellGet(prefWidth), PVRShellGet(prefHeight));
}

inline bool VShell::ReleaseView()
{
	return mApp.DestroyView();
}

inline bool VShell::RenderScene()
{
	mApp.UpdateFrame(PVRShellGetTime());
	mApp.RenderFrame(PVRShellGet(prefWidth), PVRShellGet(prefHeight));

	return true;
}

#endif