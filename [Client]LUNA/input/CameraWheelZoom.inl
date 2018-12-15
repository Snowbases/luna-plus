
//#define ZOOMRATE -80.0f
#define ZOOMRATE -1.0f
#define FLYRATE -1.0f
//////////////////////////////////////////////////////////////////////////
// Ä«¸Þ¶ó ÁÜÀÎ¾Æ¿ô
void CameraWheelZoom(CKeyboard* Keyboard,CMouse* Mouse)
{
	BOOL bResult = FALSE;
	PTRLISTPOS pos = WINDOWMGR->m_pWindowList.GetHeadPosition();

	for(cWindow* pWnd = (cWindow*)WINDOWMGR->m_pWindowList.GetNext(pos);
		0 < pWnd;
		pWnd = (cWindow*)WINDOWMGR->m_pWindowList.GetNext(pos))
	{
		if(FALSE == pWnd->IsActive())
		{
			continue;
		}
		else if(pWnd->PtInWindow(MOUSE->GetMouseX(), MOUSE->GetMouseY()))
		{
			bResult = TRUE ;
			break;
		}
	}

	if(FALSE == bResult)
	{
		if( CAMERA->GetCameraMode() == eCM_EyeView )
		{
			CAMERA->Fly(
				Mouse->GetWheel() * FLYRATE);
		}
		else
		{
			CAMERA->ZoomDistance(
				0,
				Mouse->GetWheel() * ZOOMRATE,
				0);
		}
	}
}
