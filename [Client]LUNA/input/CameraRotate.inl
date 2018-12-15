void CameraRotate(CKeyboard* Keyboard,CMouse* Mouse)
{
	static BOOL bLastRotating = FALSE;

	if( WINDOWMGR->IsDragWindow() )	//À©µµ¿ì µå·¡±×Áß¿£ Ä«¸Þ¶ó ¾È¿òÁ÷ÀÓ.
		return;

	BOOL bCurRotating = Mouse->RButtonPressed();
	if(bCurRotating)
	{
		const int x = Mouse->GetMouseAxisX();
		const int y = Mouse->GetMouseAxisY();

		if(x || y)
		{
			CAMERA->MouseRotate(x,y);
		}
	}

	bLastRotating = bCurRotating;
}








