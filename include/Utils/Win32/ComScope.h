#pragma once
#include "Win32Exceptions.h"

_WIN32_BEGIN

class ComDisposable {
public:
	virtual void DisposeComInterface_v() = 0;
};

class ComScope {
private:
	HRESULT m_hRes_;
	ComDisposable* m_pDisposable_ = nullptr;

public:
	ComScope()                = delete;
	ComScope(const ComScope&) = delete;

	/// <summary>
	/// 使用CoinitializeEx的Com交互方式来初始化ComScope，在对象销毁时，自动调用CoUninitialize
	/// </summary>
	/// <param name="dwCoinit">调用CoinitializeEx的Com交互方式</param>
	ComScope(DWORD dwCoinit)
	    : m_hRes_(::CoInitializeEx(NULL, dwCoinit)) {
		if (FAILED(m_hRes_)) {
			throw ComInteropException(m_hRes_);
		}
	}

	/// <summary>
	/// 使用指向具有与Com交互的对象的指针来初始化ComScope，在本对象销毁时，自动调用初始化的对象的DisposeComInterface_v方法
	/// </summary>
	/// <param name="p_pDisposable">指向具有与Com交互的对象的指针</param>
	ComScope(ComDisposable* p_pDisposable)
	    : m_pDisposable_(p_pDisposable) {
	}

	~ComScope() {
		if (m_pDisposable_) {
			m_pDisposable_->DisposeComInterface_v();
			return;
		}

		if (SUCCEEDED(m_hRes_)) {
			::CoUninitialize();
		}
	}
};

_WIN32_END