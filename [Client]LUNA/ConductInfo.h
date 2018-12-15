#pragma once

class CMHFile;

class cConductInfo
{
protected :
	CONDUCT_INFO m_pConductInfo ;	
public:
	cConductInfo(void);
	virtual ~cConductInfo(void);

	BOOL InitConductInfo( CMHFile* pFile ) ;

	char*	GetConductName() ;

	WORD	GetConductIdx() ;
	WORD	GetConductTooltipIdx() ;

	WORD	GetConductKind() ;
	WORD	GetConductPos() ;

	//int		GetLowImage() ;
	int		GetHighImage() ;

	int		GetMotionIdx() ;
	
	BOOL	IsInvalidOnVehicle() const;
};
