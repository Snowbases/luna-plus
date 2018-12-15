//---BROWSER
#pragma once

#include <atlbase.h>    
#include <atlcom.h>     
#include <atlhost.h>    
#include <Exdispid.h>
#include "cWindow.h"

#import <shdocvw.dll>

class DWebBrowserEventsImpl : public DWebBrowserEvents
{
	// IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (IID_IUnknown == riid || __uuidof(SHDocVw::DWebBrowserEventsPtr) == riid)
		{
			*ppv = (LPUNKNOWN)(SHDocVw::DWebBrowserEventsPtr*)this;
		}
		else if (IID_IOleClientSite == riid)
		{
			*ppv = (IOleClientSite*)this;
		}
		else if (IID_IDispatch == riid)
		{
			*ppv = (IDispatch*)this;
		}
		else
		{
			return E_NOTIMPL;
		}

		AddRef();

		return NOERROR;
	}

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 0; }

	// IDispatch methods
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) { return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)  { return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)  { return E_NOTIMPL; }
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr)
	{
		switch (dispIdMember)
		{
		case DISPID_BEFORENAVIGATE:
			{
				BeforeNavigate( _bstr_t( pDispParams->rgvarg[5].bstrVal ),
					0,
					_bstr_t( pDispParams->rgvarg[3].bstrVal ),
					NULL,
					_bstr_t(""),
					NULL);

				break;

			}
		case DISPID_NAVIGATECOMPLETE:
			{
				NavigateComplete(_bstr_t( pDispParams->rgvarg[3].bstrVal ));
				break;
			}
		}

		return NOERROR;
	}

    // Methods:
    HRESULT BeforeNavigate (_bstr_t URL, long Flags, _bstr_t TargetFrameName, VARIANT * PostData, _bstr_t Headers, VARIANT_BOOL * Cancel)
	{
		//MessageBox(NULL, (char*)URL, "BeforeNavigate", MB_OK);
		return S_OK;
	}
    HRESULT NavigateComplete ( _bstr_t URL ) 
	{ 
		//MessageBox(NULL, (char*)URL, "NavigateComplete", MB_OK);
		return S_OK; 
	}
    HRESULT StatusTextChange ( _bstr_t Text ) { return S_OK; }
    HRESULT ProgressChange (long Progress, long ProgressMax ) { return S_OK; }
    HRESULT DownloadComplete() { return S_OK; }
    HRESULT CommandStateChange (long Command, VARIANT_BOOL Enable ) { return S_OK; }
    HRESULT DownloadBegin () { return S_OK; }
    HRESULT NewWindow (_bstr_t URL, long Flags, _bstr_t TargetFrameName, VARIANT * PostData, _bstr_t Headers, VARIANT_BOOL * Processed ) { return S_OK; }
    HRESULT TitleChange ( _bstr_t Text ) { return S_OK; }
    
	HRESULT FrameBeforeNavigate (_bstr_t URL, long Flags, _bstr_t TargetFrameName, VARIANT * PostData, _bstr_t Headers, VARIANT_BOOL * Cancel ) { return S_OK; }
    HRESULT FrameNavigateComplete (_bstr_t URL ) { return S_OK; }
    HRESULT FrameNewWindow (_bstr_t URL, long Flags, _bstr_t TargetFrameName, VARIANT * PostData, _bstr_t Headers, VARIANT_BOOL * Processed ) { return S_OK; }
    
	HRESULT Quit (VARIANT_BOOL * Cancel ) { return S_OK; }
    
	HRESULT WindowMove ( ) { return S_OK; }
    HRESULT WindowResize ( ) { return S_OK; }
    HRESULT WindowActivate ( ) { return S_OK; }
    
	HRESULT PropertyChange (_bstr_t Property ) { return S_OK; }
};


//---아직 기능이 부족하다. 지금 현재 필요한 기능만 들어있음. 다음에 필요하면 확장하시오.
class cBrowser : public cWindow
{
protected:
	char						m_szURL[1024];
	DWebBrowserEventsImpl		m_events;
	CComQIPtr<IWebBrowser2>		m_pWebBrowser;
	HWND						m_hwndIE;

public:
	cBrowser();
	virtual ~cBrowser();
	virtual void Add(cWindow*) {}
	virtual void SetActive(BOOL);
	virtual void SetAbsXY(LONG x, LONG y);

	void Navigate( char* pURL );

};
