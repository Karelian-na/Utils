#include "Win32/Shell.h"

#include <filesystem>

#include <ShObjIdl.h>
#include <shellapi.h>

#include "Win32/ComScope.h"

_COM_SMARTPTR_TYPEDEF(IFileOperation, __uuidof(IFileOperation));
_COM_SMARTPTR_TYPEDEF(IShellItem, __uuidof(IShellItem));

_WIN32_BEGIN

HRESULT Shell::MoveItem(const String& pc_strSrc, const String& pc_strDest) {
	namespace fs = std::filesystem;

	ComScope scope(COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (!fs::exists(pc_strSrc)) {
		throw Exception(ERR_FILE_NOT_FOUND);
	}

	fs::create_directories(pc_strDest);
	if (!fs::exists(pc_strDest)) {
		throw Exception(TEXT("创建目标文件夹失败！"));
	}

	IFileOperationPtr pFileOper;
	HRESULT hRes = pFileOper.CreateInstance(CLSID_FileOperation);
	if (FAILED(hRes)) {
		return hRes;
	}

	hRes = pFileOper->SetOperationFlags(FOF_NOCONFIRMMKDIR | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS | FOFX_REQUIREELEVATION);
	if (SUCCEEDED(hRes)) {
		IShellItemPtr pFrom = NULL, pTo = NULL;
		hRes = ::SHCreateItemFromParsingName(pc_strSrc.c_str(), NULL, IID_PPV_ARGS(&pFrom));
		if (FAILED(hRes)) {
			return hRes;
		}

		hRes = ::SHCreateItemFromParsingName(pc_strDest.c_str(), NULL, IID_PPV_ARGS(&pTo));
		if (FAILED(hRes)) {
			return hRes;
		}

		hRes = pFileOper->MoveItem(pFrom, pTo, NULL, NULL);
		if (FAILED(hRes)) {
			return hRes;
		}
	}

	return pFileOper->PerformOperations();
}

_WIN32_END