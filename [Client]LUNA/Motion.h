#ifndef _MOTION_H_
#define _MOTION_H_

enum WEAPON_KIND
{
	WP_NONE,
	WP_ONEHANDED,
	WP_TWOHANDED,
	WP_DAGGER,
	WP_STAFF,
	WP_BOW,

	// 080708 LYW --- Motion : 무기 타입을 추가한다.
	//WP_MACE,
	WP_MUSKET,
	WP_DUALSWORD,
	//090528 ONS 신규종족 양손무기 속성 추가
	WP_DUALDAGGER,

	WP_MAX2,
};

enum eCHARACTERMOTION
{
	// 080708 LYW --- Motion : 무기 타입이 추가 되면서, 애니메이션 종류 몇가지를 삭제한다.
	//eCharacterMotion_Standard,
	//eCharacterMotion_Battle,
	//eCharacterMotion_Walk,
	//eCharacterMotion_Run,
	//eCharacterMotion_FrontDamage,
	//eCharacterMotion_LeftDamage,
	//eCharacterMotion_RightDamage,

	//eCharacterMotion_Die1,
	//eCharacterMotion_Died1,
	//eCharacterMotion_Die2,
	//eCharacterMotion_Died2,

	//eCharacterMotion_RestStart,
	//eCharacterMotion_Rest,
	//eCharacterMotion_RestEnd,
	//eCharacterMotion_RestDamage,

	//eCharacterMotion_Max,

	eCharacterMotion_Standard,
	eCharacterMotion_Battle,
	eCharacterMotion_Walk,
	eCharacterMotion_Run,
	eCharacterMotion_FrontDamage,

	eCharacterMotion_Die1,
	eCharacterMotion_Died1,
	eCharacterMotion_Die2,
	eCharacterMotion_Died2,

	eCharacterMotion_RestStart,
	eCharacterMotion_Rest,
	eCharacterMotion_RestEnd,

	eCharacterMotion_Max,
};

enum eWingMotion
{
	eWingMotion_None,
	eWingMotion_Idle,
	eWingMotion_Walk,
	eWingMotion_Run,
	eWingMotion_Hit,
	eWingMotion_Die1,
	eWingMotion_Died1,
	eWingMotion_Die2,
	eWingMotion_Died2,
	eWingMotion_Max,
};

static WORD CHARACTER_MOTION[ eCharacterMotion_Max ][ WP_MAX2 ] =

{
	// 080708 LYW --- Motion : 무기타입 추가에 따른 에니메이션 번호를 변경한다.
	//{	191, 192, 193, 195, 196, 194, 192 },
	//{	1,	2,	3,	5,	6,	4,	7	},
	//{	8,	9,	10,	12,	13,	11,	14	},
	//{	15,	16,	17,	19,	20,	18,	21	},
	//{	22,	23,	24,	26,	27,	25,	28	},
	//{	29,	30,	31,	33,	34,	32,	35	},
	//{	36,	37,	38,	40,	41,	39,	42	},
	//{	43,	44,	45,	47,	48,	46,	49	},
	//{	50,	51,	52,	54,	55,	53,	56	},
	//{	57,	58,	59,	61,	62,	60,	63	},
	//{	64,	65,	66,	68,	69,	67,	70	},
	//{	71,	72,	73,	75,	76,	74,	77	},
	//{	78,	79,	80,	82,	83,	81,	84	},
	//{	85,	86,	87,	89,	90,	88,	91	},
	//{	92,	93,	94,	96,	97,	95,	98	}

	// 090530 ONS 신규종족관련 모션 추가(9번째 column), 임시로 DUALSWORD와 동일하게 설정
	{ 111, 112, 113, 115, 116, 114, 117, 118, 119 },		// eCharacterMotion_Standard
	{   1,   2,   3,   5,   6,   4,   7,   8,   9 },		// eCharacterMotion_Battle
	{  11,  12,  13,  15,  16,  14,  17,  18,  19 },		// eCharacterMotion_Walk 
	{  21,  22,  23,  25,  26,  24,  27,  28,  29 },		// eCharacterMotion_Run
	{  31,  32,  33,  35,  36,  34,  37,  38,  39 },		// eCharacterMotion_FrontDamage
	{  41,  42,  43,  45,  46,  44,  47,  48,  49 },		// eCharacterMotion_Die1
	{  51,  52,  53,  55,  56,  54,  57,  58,  59 },		// eCharacterMotion_Died1
	{  61,  62,  63,  65,  66,  64,  67,  68,  69 },		// eCharacterMotion_Die2
	{  71,  72,  73,  75,  76,  74,  77,  78,  79 },		// eCharacterMotion_Died2
	{  81,  82,  83,  85,  86,  84,  87,  88,  89 },		// eCharacterMotion_RestStart
	{  91,  92,  93,  95,  96,  94,  97,  98,  99 },		// eCharacterMotion_Rest
	{ 101, 102, 103, 105, 106, 104, 107, 108, 109 }			// eCharacterMotion_RestEnd
};

// 080708 LYW --- Motion : 모션 번호 수정.
//#define Motion_SelBefore	272
//#define Motion_SelAfter		273
//#define Motion_SelAfterstop	274
//#define Motion_SelBack		275
#define Motion_SelBefore	121
#define Motion_SelAfter		122
#define Motion_SelAfterstop	123
#define Motion_SelBack		124

enum eMOTION
{
	eMotion_Peace_Standard					 = 1,
	eMotion_Peace_Walk							,
	eMotion_Peace_Run							,
	
	eMotion_Battle_Gum_Standard					,
	eMotion_Battle_Gwun_Standard				,
	eMotion_Battle_Do_Standard					,
	eMotion_Battle_Chang_Standard				,
	eMotion_Battle_Gung_Standard				,
	eMotion_Battle_Amgi_Standard				,

	eMotion_Battle_Gum_Walk						,	//10
	eMotion_Battle_Gwun_Walk					,
	eMotion_Battle_Do_Walk						,
	eMotion_Battle_Chang_Walk					,
	eMotion_Battle_Gung_Walk					,
	eMotion_Battle_Amgi_Walk					,
	
	eMotion_Battle_Gum_Run						,
	eMotion_Battle_Gwun_Run						,
	eMotion_Battle_Do_Run						,
	eMotion_Battle_Chang_Run					,
	eMotion_Battle_Gung_Run						,	//20
	eMotion_Battle_Amgi_Run						,

	eMotion_Damage_Left							,
	eMotion_Damage_Right						,
	eMotion_Damage_Front						,

	eMotion_Die_Fly								,	//25
	eMotion_Die_Normal							,

	eMotion_KyungGong1_Start_NoWeapon			,
	eMotion_KyungGong1_Move_NoWeapon			,
	eMotion_KyungGong1_End_NoWeapon				,

	eMotion_KyungGong2_Start_NoWeapon			,	//30
	eMotion_KyungGong2_Move_NoWeapon			,
	eMotion_KyungGong2_End_NoWeapon				,

	eMotion_KyungGong3_Start_NoWeapon			,
	eMotion_KyungGong3_Move_NoWeapon			,
	eMotion_KyungGong3_End_NoWeapon				,	//35

	eMotion_Ungijosik_Start						,
	eMotion_Ungijosik_Ing						,
	eMotion_Ungijosik_End						,
	
	eMotion_Ungijosik_Damage					,

	// 070504 LYW --- Motion : Modified motion number.
	//eMotion_StreetStall_Start		= 77 ,	//40
	eMotion_StreetStall_Start		= 71 ,	//40
	eMotion_StreetStall_Ing			= 78 ,
	eMotion_StreetStall_End			= 85 ,
	
	eMotion_Died_Normal							,
	eMotion_Died_Fly							,

	eMotion_Defence_Gum							,	//45
	eMotion_Defence_Gwon						,
	eMotion_Defence_Do							,
	eMotion_Defence_Chang						,
	eMotion_Defence_Gung						,
	eMotion_Defence_Amgi						,	//50
		
	eMotion_Peace_Standard_Do					,
	eMotion_Peace_Standard_Change				,
	eMotion_Peace_Walk_Do						,
	eMotion_Peace_Walk_Change					,
	eMotion_Peace_Run_Do						,	//55
	eMotion_Peace_Run_Change					,

	// 중간에 공격동작들이 있는데 그건 무공리스트에서 직접 입력하기때문에 필요없다.

//커뮤니티 동작
	eMotion_Society_Bow							= 251,
	eMotion_Society_Happy						,
	eMotion_Society_Sad							,
	eMotion_Society_Yes							,	//60
	eMotion_Society_No							,
	eMotion_Society_Sit_Start					,
	eMotion_Society_Sit_Ing						,
	eMotion_Society_Stand						,
	eMotion_Society_BigBow						,

// 아이템사용 애니
	eMotion_Item_Dyeing_1						= 260,	// 검, 권, 궁, 암기		//65
	eMotion_Item_Dyeing_2						,		// 도
	eMotion_Item_Dyeing_3						,		// 창
	eMotion_Item_ChangeFace_1					,		// 검, 권, 궁, 암기
	eMotion_Item_ChangeFace_2					,		// 도
	eMotion_Item_ChangeFace_3					,		// 창					//70
	eMotion_Item_ChangeHair_1					,		// 검, 권, 궁, 암기
	eMotion_Item_ChangeHair_2					,		// 도
	eMotion_Item_ChangeHair_3					,		// 창
	eMotion_Item_Teleport_1						,		// 검, 권, 궁, 암기
	eMotion_Item_Teleport_2						,		// 도					//75
	eMotion_Item_Teleport_3						,		// 창


// pjs [3/11/2003]	로그인 행동들...
	// 061214 LYW --- Modified motion number for test.
	/*
	eMotion_Sel1Before						 = 290, // PJS 애니가 맞는지 시험  [5/15/2003]
	eMotion_sel1After						 ,
	eMotion_Sel1Afterstop					 ,
	eMotion_Sel1Back						 ,
	*/
	// 061226 LYW --- Change motion character.
	/*
	eMotion_Sel1Before						 = 37, // PJS 애니가 맞는지 시험  [5/15/2003]
	eMotion_sel1After						 = 42,
	eMotion_Sel1Afterstop					 = 2,
	eMotion_Sel1Back						 = 40,								//80

	eMotion_Sel2Before						 = 294,
	eMotion_Sel2After						 = 295,
	eMotion_Sel2Afterstop					 = 296,
	eMotion_Sel2Back						 = 297,

	eMotion_Sel3Before						 ,	//85
	eMotion_Sel3After						 ,
	eMotion_Sel3Afterstop					 ,
	eMotion_Sel3Back						 ,

	eMotion_Sel4Before						 ,
	eMotion_Sel4After						 ,	//90
	eMotion_Sel4Afterstop					 ,
	eMotion_Sel4Back						 ,

	eMotion_Sel5Before						 = 37,
	eMotion_Sel5After						 = 42,
	eMotion_Sel5Afterstop					 = 2,	//95
	eMotion_Sel5Back						 = 40,
	*/
	// 080708 LYW --- Motion : 모션 번호 수정.
	//eMotion_Sel1Before						 = 272,		// PJS 애니가 맞는지 시험  [5/15/2003]
	//eMotion_sel1After						 = 273,
	//eMotion_Sel1Afterstop					 = 274,
	//eMotion_Sel1Back						 = 275,		//80

	//eMotion_Sel2Before						 = 272,	
	//eMotion_Sel2After						 = 273,
	//eMotion_Sel2Afterstop					 = 274,
	//eMotion_Sel2Back						 = 275,	

	//eMotion_Sel3Before						 = 272,	//85
	//eMotion_Sel3After						 = 273,
	//eMotion_Sel3Afterstop					 = 274,
	//eMotion_Sel3Back						 = 275,	

	//eMotion_Sel4Before						 = 272,	
	//eMotion_Sel4After						 = 273,	//90
	//eMotion_Sel4Afterstop					 = 274,
	//eMotion_Sel4Back						 = 275,	

	//eMotion_Sel5Before						 = 272,	
	//eMotion_Sel5After						 = 273,
	//eMotion_Sel5Afterstop					 = 274,//95
	//eMotion_Sel5Back						 = 275,	


	eMotion_Sel1Before						 = 121,		// PJS 애니가 맞는지 시험  [5/15/2003]
	eMotion_sel1After						 = 122,
	eMotion_Sel1Afterstop					 = 123,
	eMotion_Sel1Back						 = 124,		//80

	eMotion_Sel2Before						 = 121,	
	eMotion_Sel2After						 = 122,
	eMotion_Sel2Afterstop					 = 123,
	eMotion_Sel2Back						 = 124,	

	eMotion_Sel3Before						 = 121,	//85
	eMotion_Sel3After						 = 122,
	eMotion_Sel3Afterstop					 = 123,
	eMotion_Sel3Back						 = 124,	

	eMotion_Sel4Before						 = 121,	
	eMotion_Sel4After						 = 122,	//90
	eMotion_Sel4Afterstop					 = 123,
	eMotion_Sel4Back						 = 124,	

	eMotion_Sel5Before						 = 121,	
	eMotion_Sel5After						 = 122,
	eMotion_Sel5Afterstop					 = 123,//95
	eMotion_Sel5Back						 = 124,	



		
	eMotion_Tactic_Start						= 330,
	eMotion_Tactic_Ing							= 331,
	eMotion_Tactic_Excute						= 332,
/*
	eMotion_Emoticon_Bow = 150,//인사
	eMotion_Emoticon_Thanks = 151,//감사
	eMotion_Emoticon_Yes = 152,//긍정
	eMotion_Emoticon_No = 153,//부정
	eMotion_Emoticon_Pleasure = 154,//기쁨
	eMotion_Emoticon_Sadness = 155,//슬픔
	eMotion_Emoticon_Love = 156,//사랑
	eMotion_Emoticon_Hostility = 157,//적대
	eMotion_Emoticon_Disappoint = 158,//실망
	eMotion_Emoticon_Angry = 159,//분노
	eMotion_Emoticon_Suprised = 160,//놀람
	eMotion_Emoticon_Evasion = 161,//회피
	eMotion_Emoticon_Declare = 162,//선언
*/	
	// 특수무공 동작들도 공격 동작과 마찬가지로 처리

};


//---KES ETC 071021
//---enum 순서 수정
enum eMONSTERMOTION
{
	eMonsterMotion_Standard,	//기본1
	eMonsterMotion_Standard2,	//기본2
	eMonsterMotion_Standard3,	//전투 아이들
	eMonsterMotion_Walk,
	eMonsterMotion_Run, 
	eMonsterMotion_Attack1,
	eMonsterMotion_Attack2,
	eMonsterMotion_Attack3,
	eMonsterMotion_Attack4,
	eMonsterMotion_Attack5,
	eMonsterMotion_DamageFront,
	eMonsterMotion_DamageLeft,		//사용안함
	eMonsterMotion_DamageRight,		//사용안함
	eMonsterMotion_Die,				// 제자리에서 죽기
	eMonsterMotion_Died,			// 죽어서 멈춰있기
	eMonsterMotion_FlyDie,			// 날라가서 죽기
	eMonsterMotion_FlyDied,		// 날라가서 죽어서 멈춰있기
};
/*
enum eBOSSMONSTERMOTION
{
	eBossMonsterMotion_Stand		= 1,
	eBossMonsterMotion_Walk			= 2,
	
	eBossMonsterMotion_Attack1		= 3,		// 일반 공격
	eBossMonsterMotion_Attack2		= 4,		// 소환
	eBossMonsterMotion_Attack3		= 5,		// 찍기

	eBossMonsterMotion_Attack4		= 6,		// NULL
	eBossMonsterMotion_Attack5		= 7,		// NULL

	eBossMonsterMotion_Die			= 8,
	eBossMonsterMotion_Died			= 9,
	
	eBossMonsterMotion_RestDown		= 10,
	eBossMonsterMotion_RestOn		= 11,
	eBossMonsterMotion_RestUp		= 12,
};*/
enum eBOSSMONSTERMOTION
{
	eBossMonsterMotion_Stand		= 1,
	eBossMonsterMotion_Stand2		= 2,
	eBossMonsterMotion_Walk			= 3,

	eBossMonsterMotion_Run1			= 4,
	eBossMonsterMotion_Run2			= 5,
	eBossMonsterMotion_Run3			= 6,
	
	eBossMonsterMotion_Attack1		= 7,
	eBossMonsterMotion_Attack2		= 8,
	eBossMonsterMotion_Attack3		= 9,

	eBossMonsterMotion_Attack4		= 10,
	eBossMonsterMotion_Attack5		= 11,
	eBossMonsterMotion_Attack6		= 12,
	eBossMonsterMotion_Summon		= 13,

	eBossMonsterMotion_Recovery		= 14,

	eBossMonsterMotion_Die			= 15,
	eBossMonsterMotion_Died			= 16,

	eBossMonsterMotion_Attack7		= 17,

	eBossMonsterMotion_Advent		= 18,
};

#endif
