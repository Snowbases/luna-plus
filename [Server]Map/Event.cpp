#include "StdAfx.h"
#include ".\Event.h"

CEvent::CEvent( CEvent::Type type, eStatusKind targetStatus ) :
mType( type ),
mTargetStatus( targetStatus )
{}

// 080616 LUJ, 다른 오브젝트에게 피해를 줬을 때 발동되는 이벤트
CGiveDamageEvent::CGiveDamageEvent( CObject* target, const RESULTINFO& result ) :
CEvent( TypeGiveDamage, eStatusKind_None ),
mResult( result ),
mTarget( target )
{}

CGiveDamageEvent::CGiveDamageEvent( CObject* target, const RESULTINFO& result, eStatusKind targetStatus ) :
CEvent( TypeGiveDamage, targetStatus ),
mResult( result ),
mTarget( target )
{}

// 080616 LUJ, 다른 오브젝트가 피해를 받았을 때 발동되는 이벤트
CTakeDamageEvent::CTakeDamageEvent( CObject* attacker, RESULTINFO& result ) :
CEvent( TypeTakeDamage, eStatusKind_None ),
mResult( result ),
mAttacker( attacker )
{}

CTakeDamageEvent::CTakeDamageEvent( CObject* attacker, RESULTINFO& result, eStatusKind targetStatus ) :
CEvent( TypeTakeDamage, targetStatus ),
mResult( result ),
mAttacker( attacker )
{}

// 080616 LUJ, 다른 오브젝트에게 피살될 때 발생하는 이벤트
// 080708 LUJ, 피해자를 인자로 받도록 함
CDieEvent::CDieEvent( CObject* killer, CObject* victim ) :
CEvent( TypeDie, eStatusKind_None ),
mKiller( killer ),
mVictim( victim )
{}

// 080616 LUJ, 다른 오브젝트를 살해할 때 발생하는 이벤트
// 080708 LUJ, CDieEvent와 같은 구조를 쓰도록 함
CKillEvent::CKillEvent( CObject* killer, CObject* victim ) :
CDieEvent( killer, victim )
{
	mType = TypeKill;
}