#include "CvGameCoreDLLPCH.h"
#include "CvExe.h"
#include "CvExeBuild.h"

// must be included after all other headers
#include "LintFree.h"

//------------------------------------------------------------------------------
const char* Exe::GetBuildName()
{
	return ExeBuild::GetName(ExeBuild::GetType());
}
