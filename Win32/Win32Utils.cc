#include "Win32/Win32Utils.h"

#include <TlHelp32.h>
#include <netlistmgr.h>

#include <mutex>

#ifdef NOMINMAX
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#include <gdiplus.h>
#undef max
#undef min
#endif // NOMINMAX

#include <dwrite.h>

_COM_SMARTPTR_TYPEDEF(INetworkListManager, __uuidof(INetworkListManager));

_WIN32_BEGIN

UTILS_API const TCHAR* GetComError(HRESULT p_hRes, IErrorInfo* p_pErrInfo) {
	static std::mutex s_mtxEntirely;
	static String s_strEntirely;
	std::lock_guard<std::mutex> guard(s_mtxEntirely);
	s_strEntirely.clear();
	if (p_pErrInfo) {
		BSTR msg;
		if (SUCCEEDED(p_pErrInfo->GetDescription(&msg)) && msg) {
			s_strEntirely = msg;
		};
	}

	if (ERROR_SUCCESS != p_hRes) {
		String&& msg = StringUtils::Trim(GetErrorString(p_hRes), { String(TEXT("。")) });
		s_strEntirely += FORMAT("{}(0x{:X})", msg, (unsigned long)p_hRes);
	}

	return s_strEntirely.c_str();
}

UTILS_API std::wstring GetErrorStringW(LSTATUS p_lMsgId) {
	TCHAR pcErrorMessage[1024] = { 0 };
	DWORD dwCopiedLen          = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, p_lMsgId, 0, pcErrorMessage, 1024, NULL);
	return pcErrorMessage;
}

UTILS_API std::string GetErrorStringA(LSTATUS p_lMsgId) {
	char pcErrorMessage[1024] = { 0 };
	DWORD dwCopiedLen         = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, p_lMsgId, 0, pcErrorMessage, 1024, NULL);
	return pcErrorMessage;
}

UTILS_API bool ExistsProcess(const String& pc_strProcessName) {
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapShot == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32 pe32 {};
	pe32.dwSize = sizeof(PROCESSENTRY32);
	::Process32First(hSnapShot, &pe32);
	do {
		if (pc_strProcessName == pe32.szExeFile) {
			return true;
		}
	} while (::Process32Next(hSnapShot, &pe32));

	return false;
}

UTILS_API bool IsConnectedToInternet() {
	INetworkListManagerPtr pNetMgr = nullptr;
	HRESULT hRes                   = pNetMgr.CreateInstance(CLSID_NetworkListManager);

	if (!SUCCEEDED(hRes)) {
		return false;
	}

	VARIANT_BOOL varbool;
	hRes = pNetMgr->get_IsConnectedToInternet(&varbool);

	return varbool;
}

UTILS_API StringList GetSystemFonts() {
	struct IComPtrDeleter {
		void operator()(IUnknown* pIUnknown) {
			if (pIUnknown) {
				pIUnknown->Release();
				pIUnknown = nullptr;
			}
		}
	};

	std::unique_ptr<IDWriteFactory, IComPtrDeleter> pDWriteFactory = nullptr;

	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));

	std::unique_ptr<IDWriteFontCollection, IComPtrDeleter> pFontCollection = nullptr;

	// Get the system font collection.
	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->GetSystemFontCollection(reinterpret_cast<IDWriteFontCollection**>(&pFontCollection));
	}

	size_t familyCount = 0;

	// Get the number of font families in the collection.
	if (SUCCEEDED(hr)) {
		familyCount = pFontCollection->GetFontFamilyCount();
	}

	StringList vstrFonts;
	for (size_t idx = 0; idx < familyCount; ++idx) {
		std::unique_ptr<IDWriteFontFamily, IComPtrDeleter> pFontFamily = nullptr;

		// Get the font family.
		if (SUCCEEDED(hr)) {
			hr = pFontCollection->GetFontFamily(idx, reinterpret_cast<IDWriteFontFamily**>(&pFontFamily));
		}

		std::unique_ptr<IDWriteLocalizedStrings, IComPtrDeleter> pFamilyNames = nullptr;

		// Get a list of localized strings for the family name.
		if (SUCCEEDED(hr)) {
			hr = pFontFamily->GetFamilyNames(reinterpret_cast<IDWriteLocalizedStrings**>(&pFamilyNames));
		}

		size_t index = 0;
		BOOL exists  = false;

		wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

		if (SUCCEEDED(hr)) {
			// Get the default locale for this user.
			int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

			// If the default locale is returned, find that locale name, otherwise use "en-us".
			if (defaultLocaleSuccess) {
				hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
			}
			if (SUCCEEDED(hr) && !exists) // if the above find did not find a match, retry with US English
			{
				hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
			}
		}

		// If the specified locale doesn't exist, select the first on the list.
		if (!exists)
			index = 0;

		size_t length = 0;

		// Get the string length.
		if (SUCCEEDED(hr)) {
			hr = pFamilyNames->GetStringLength(index, &length);
		}

		std::vector<TCHAR> name(length + 1);

		// Get the family name.
		if (SUCCEEDED(hr)) {
			hr = pFamilyNames->GetString(index, name.data(), length + 1);
		}
		if (SUCCEEDED(hr)) {
			vstrFonts.push_back(name.data());
		}
	}
	return vstrFonts;
}

_WIN32_END
