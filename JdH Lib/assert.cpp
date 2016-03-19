#include "CvGameCoreDLLPCH.h"

#include "jdh\resource.h"

#include "jdh\assert.h"
#include "jdh\log.h"


#ifdef NOUSER
#	error NOUSER must not be defined in CvGameCoreDLLUtil_Win32Headers.h
#endif
#ifdef NOCTLMGR
#	error NOCTLMGR must not be defined in CvGameCoreDLLUtil_Win32Headers.h
#endif
#ifdef NOWINMESSAGES
#	error NOWINMESSAGES must not be defined in CvGameCoreDLLUtil_Win32Headers.h
#endif
#ifndef DialogBox
#	error DialogBox not defined!
#endif


namespace jdh
{
	namespace impl
	{
		HMODULE g_hGDLL = NULL;
		bool g_bIsAssertDisabled = false;
		bool g_bIsAssertLogDisabled = false;

		INT_PTR CALLBACK AssertDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_INITDIALOG:
			{
				SetDlgItemText(hDlg, IDC_JDH_ASSERT_TEXT_1, reinterpret_cast<PCSTR>(lParam));
			} break;
			case WM_COMMAND:
			{
				INT_PTR result = 0;
				switch (LOWORD(wParam))
				{
				case IDOK:
					result = 1;
					break;
				case IDIGNORE:
					result = 2;
					break;
				case IDABORT:
					result = 3;
					break;
				}
				if (result)
				{
					if (IsDlgButtonChecked(hDlg, IDC_JDH_ASSERT_IGNORE_ALLWAYS))
					{
						result += 10;
					}

					if (EndDialog(hDlg, result))
						return TRUE;
				}
			} break;
			}
			return FALSE;
		}
		bool AssertDlg(_In_ PCSTR szExpr, _In_ PCSTR szFile, unsigned int line, _Inout_ bool& bIgnoreAlways, _In_opt_ PCSTR szMsg)
		{
			std::string& tmpbuffer = GetTempString();
			append(tmpbuffer, "assert failed (", szExpr, ") in ", szFile, "(", line, ')');
			if (szMsg)
			{
				append(tmpbuffer, ": ", szMsg);
			}
#ifdef __JDHLOGFILE
			if (!::jdh::impl::g_bIsAssertLogDisabled)
			{
				LogLockGuard lock;
				std::string& buffer = LogFileBuffer(ERROR, lock);
				append(buffer, tmpbuffer);
				Log(buffer);
			}
#endif
			if (bIgnoreAlways || g_bIsAssertDisabled)
				return false;
			INT_PTR result = DialogBoxParam(g_hGDLL, MAKEINTRESOURCE(IDD_JDH_ASSERT_DLG), NULL, AssertDlgProc, reinterpret_cast<LPARAM>(tmpbuffer.c_str()));
			if (result > 10)
			{
				result -= 10;
				bIgnoreAlways = true;
			}
			switch (result)
			{
			case 1:
				return true;
			case 2:
				return false;
			case 3:
				ExitProcess(1);
			default:
				JDHLOG(ERROR, "Creating AssertDlg failed: ", GetLastError());
				return true;
			}
		}
	}
}

