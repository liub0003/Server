
#ifndef __GAMEUTIL_H__
#define __GAMEUTIL_H__

#include "Type.h"
//#include "GameDefine.h"
//#include "GameStruct.h"

#define __PI			3.1415f
#define __HALF_PI		__PI / 2
#define __QUARTER_PI	__PI / 4

///////////////////////////////////////////////////////////////////////
//����������ģ�鶨��
///////////////////////////////////////////////////////////////////////

//������
#if defined(__WINDOWS__)
class MyLock
{
	CRITICAL_SECTION m_Lock ;
public :
	MyLock( ){ InitializeCriticalSection(&m_Lock); } ;
	~MyLock( ){ DeleteCriticalSection(&m_Lock); } ;
	VOID	Lock( ){ EnterCriticalSection(&m_Lock); } ;
	VOID	Unlock( ){ LeaveCriticalSection(&m_Lock); } ;
};
#elif defined(__LINUX__)
class MyLock
{
	pthread_mutex_t 	m_Mutex; 
public :
	MyLock( ){ pthread_mutex_init( &m_Mutex , NULL );} ;
	~MyLock( ){ pthread_mutex_destroy( &m_Mutex) ; } ;
	VOID	Lock( ){ pthread_mutex_lock(&m_Mutex); } ;
	VOID	Unlock( ){ pthread_mutex_unlock(&m_Mutex); } ;
};
#endif
//�Զ�����������
class AutoLock_T
{
public:
	AutoLock_T(MyLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->Lock();
	}
	~AutoLock_T()
	{
		m_pLock->Unlock();
	}
protected:
private:
	AutoLock_T();
	MyLock* m_pLock;
};

class CMyTimer  
{
private:
	UINT m_uTickTerm;
	UINT m_uTickOld;

public:
	BOOL m_bOper;

public:	
	CMyTimer()
	{
		CleanUp() ;
	}

	BOOL IsSetTimer( ){ return m_bOper ; }
	
	VOID SetTermTime( UINT uTerm ){ m_uTickTerm =uTerm; }
	UINT GetTermTime( ){ return m_uTickTerm ; }

	UINT GetTickOldTime( ){ return m_uTickOld; }
	
	VOID CleanUp( )
	{ 
		m_uTickTerm = 0 ;
		m_bOper = FALSE ;
		m_uTickOld = 0 ;
	}

	VOID BeginTimer(UINT uTerm, UINT uNow)
	{
		m_bOper = TRUE;
		m_uTickTerm =uTerm;	
		m_uTickOld =uNow;
	}

	BOOL CountingTimer(UINT uNow)
	{
		if(!m_bOper)
			return FALSE;

		UINT uNew =uNow;

		if(uNew<m_uTickOld+m_uTickTerm )
			return FALSE;

		m_uTickOld =uNew;

		return TRUE;
	}
	UINT GetLeaveTime(UINT uNow)//ʣ��ʱ��;
	{
		if(!CountingTimer(uNow))
		{
			return m_uTickTerm+m_uTickOld-uNow;
		}
		return 0;
	}
};

///////////////////////////////////////////////////////////////////////
//���������Ժ�������
///////////////////////////////////////////////////////////////////////

//��ǰ�̹߳���һ��ʱ��
extern VOID			MySleep( UINT millionseconds=0 ) ;

extern TID			MyGetCurrentThreadID( ) ;

extern CHAR*		MySocketError( ) ;

extern UINT			MyCRC( const CHAR* szString ) ;

//////////////////////////////////////////////////////////////////////////
//��ѧ��������
//////////////////////////////////////////////////////////////////////////
template<class T>

INT Float2Int(T TValue)
{
	INT iValue = (INT)TValue;

	if(TValue-iValue<0.500000f)
	{
		return iValue;
	}
	else
		return iValue+1;

}

struct Flag64 
{
	UINT	m_uLowFlags ;
	UINT	m_uHighFlags ;

	Flag64()
	{
		CleanUp( ) ;
	}

	BOOL isSetBit( INT bit ) const
	{
		if(bit<32)
		{
			if ( m_uLowFlags & (1<<bit) )
				return TRUE;
		}
		else
		{
			if ( m_uHighFlags & (1<<(bit-32)) )
				return TRUE;
		}

		return FALSE;
	}

	VOID UpdateBits( INT bit, BOOL& bUpdate )
	{
		if(bit<32)
		{
			if ( bUpdate )
				m_uLowFlags |= (1<<(INT)bit);
			else
				m_uLowFlags &= (~(1<<(INT)bit));
		}
		else
		{
			if ( bUpdate )
				m_uHighFlags |= (1<<(INT)(bit-32));
			else
				m_uHighFlags &= (~(1<<(INT)(bit-32)));
		}
	}

	VOID CleanUp( )
	{
		m_uLowFlags	=	0;
		m_uHighFlags =  0;
	}
};

#define		SM_FREE					0x00	//�����ڴ����
#define		SM_C_READ				0x01	//�����ڴ��Լ���ȡ
#define		SM_C_WRITE				0x02	//�����ڴ��Լ�д
#define		SM_S_READ				0x03	//Server��
#define		SM_S_WRITE				0x04	//Serverд
#define		SM_W_READ				0x05	//World ��
#define		SM_W_WRITE				0x06	//World д


VOID		sm_lock(CHAR&	flag,CHAR type);
VOID		sm_unlock(CHAR& flag,CHAR type);
BOOL		sm_trylock(CHAR& flag,CHAR type);





#define		SM_LOCK(flag,type)				sm_lock(flag,type);
#define		SM_UNLOCK(flag,type)			sm_unlock(flag,type);
													





struct Flag16
{
	WORD m_wFlags;

	Flag16()
	{
		CleanUp( ) ;
	}

	BOOL isSetBit( INT bit ) const
	{
		if( m_wFlags & (1<<bit) )
			return TRUE;

		return FALSE;
	}

	VOID UpdateBits( INT bit, BOOL& bUpdate )
	{
		if ( bUpdate )
			m_wFlags |= (1<<bit);
		else
			m_wFlags &= (~(1<<bit));
	}

	VOID CleanUp( )
	{
		m_wFlags = 0 ;
	};
};


//���Զ��峤�ȵ�λ��Ǽ���
template <UINT nSize>
	class BitFlagSet_T
{
	public:
		enum
		{
			BIT_SIZE = nSize, //λ��ǵĳ��ȣ���λ�Ƕ�����λ
			BYTE_SIZE = 1+BIT_SIZE/8, //��Ϣ��ռ�õ��ֽ���
		};
		BitFlagSet_T(VOID)
		{
			memset((VOID*)m_aBitFlags, '\0', sizeof(m_aBitFlags));
		};
		//���ƹ�����
		BitFlagSet_T(BitFlagSet_T const& rhs)
		{
			memcpy((VOID*)m_aBitFlags, (VOID*)(rhs.GetFlags()), sizeof(m_aBitFlags));
		};
		~BitFlagSet_T() {};
		//���Ʋ�����
		BitFlagSet_T& operator=(BitFlagSet_T const& rhs)
		{
			memcpy((VOID*)m_aBitFlags, (VOID*)(rhs.GetFlags()), sizeof(m_aBitFlags));
			return *this;
		};
		//�������б��λ
		VOID MarkAllFlags(VOID)
		{
			memset((VOID*)m_aBitFlags, 0xFF, sizeof(m_aBitFlags));
		};
		//������б��λ
		VOID ClearAllFlags(VOID)
		{
			memset((VOID*)m_aBitFlags, 0x00, sizeof(m_aBitFlags));
		};
		//ȡָ���ı��λ
		BOOL GetFlagByIndex(INT const nIdx) const
		{
			if(0>nIdx||BIT_SIZE<=nIdx)
			{
				AssertEx(FALSE,"[BitFlagSet_T::GetFlagByIndex]: Index out of range!");
				return FALSE;
			}
			unsigned int nIndex = nIdx;
			return 0!=(m_aBitFlags[nIdx>>3]&(char)(1<<nIdx%8));
		}
		//���ָ���ı��λ
		void ClearFlagByIndex(INT const nIdx)
		{
			if(0>nIdx||BIT_SIZE<=nIdx)
			{
				AssertEx(FALSE,"[BitFlagSet_T::ClearFlagByIndex]: Index out of range!");
				return;
			}
			m_aBitFlags[nIdx>>3] &= ~(0x01<<(nIdx%8));
		}
		//�趨ָ���ı��λ
		VOID MarkFlagByIndex(INT const nIdx)
		{
			if(0>nIdx||BIT_SIZE<=nIdx)
			{
				AssertEx(FALSE,"[BitFlagSet_T::MarkFlagByIndex]: Index out of range!");
				return;
			}
			m_aBitFlags[nIdx>>3] |=	0x01<<(nIdx%8);
		}
		//��ռ�õ��ֽ���
		UINT GetByteSize(VOID) const {return BYTE_SIZE;}
		//��֧�ֵı����
		UINT GetBitSize(VOID) const {return BIT_SIZE;}
		//ȡ��������ָ��
		CHAR const* const GetFlags(VOID) const {return m_aBitFlags;}
	protected:
	private:
		CHAR m_aBitFlags[BYTE_SIZE]; //���ݴ洢��
};
//�Զ������ڴ�ռ�õ�vector
template <typename U, size_t nSizeLimit>
	class ResizableVector_T
{
	public:
		enum
		{
			SIZE_LIMIT = nSizeLimit, //��Ϣ�����Ԫ��������Ҫ����������������������
		};
		ResizableVector_T(VOID) : m_nCurrentSize(0), m_pUnits(NULL)
		{
		};
		~ResizableVector_T()
		{
			SAFE_DELETE_ARRAY(m_pUnits);
		};
		//Init Function
		BOOL Init(size_t nSize)
		{
			if(NULL!=m_pUnits)
			{
				AssertEx(FALSE,"[ResizableVector_T::Init]: Init a Used vector!!Check it now.");
				return FALSE;
			}
			if(SIZE_LIMIT<nSize)
			{
				AssertEx(FALSE,"[ResizableVector_T::Init]: Size over flow!! check it now.");
				return FALSE;
			}
			m_pUnits = new U[nSize];
			if(NULL==m_pUnits)
			{
				AssertEx(FALSE,"[ResizableVector_T::Init]: Allocate memory Failed!!Check it now.");
				return FALSE;
			}
			m_nCurrentSize = nSize;
			return TRUE;
		}
		BOOL Resize(size_t nSize)
		{
			if(SIZE_LIMIT<nSize)
			{
				AssertEx(FALSE,"[ResizableVector_T::Resize]: Size over flow!! check it now.");
				return FALSE;
			}
			U* pNewBlock = new U[nSize];
			if(NULL==pNewBlock)
			{
				AssertEx(FALSE,"[ResizableVector_T::Resize]: new memory Failed!!Check it now.");
				return FALSE;
			}
			//calculate the memcpy size
			size_t nCount = 0;
			if(m_nCurrentSize > nSize)
			{
				nCount = nSize;
			}
			else
			{
				nCount = m_nCurrentSize;
			}
			//copy data to new block
			memcpy((VOID*)pNewBlock, (VOID const*)m_pUnits, sizeof(U)*nCount);
			//delete old block
			SAFE_DELETE_ARRAY(m_pUnits);
			//refer to new block
			m_pUnits = pNewBlock;
			//Update new size
			m_nCurrentSize = nSize;
			return TRUE;
		}
		U const* GetUnitByIndex(size_t nIdx) const
		{
			if(0<=nIdx && m_nCurrentSize>nIdx)
			{
				return m_pUnits[nIdx];
			}
			return NULL;
		}
		BOOL SetUnitByIndex(size_t nIdx, U const& rU)
		{
			if(0<=nIdx && m_nCurrentSize>nIdx)
			{
				m_pUnits[nIdx] = rU;
				return TRUE;
			}
			return FALSE;
		}
		//��ǰ�����鳤��
		size_t GetCurrentVectorSize(VOID) const {return m_nCurrentSize;}
		//��ǰռ�õ��ڴ��ֽ���
		UINT GetByteSize(VOID) const {return sizeof(U)* m_nCurrentSize;}
	protected:
	private:
		size_t m_nCurrentSize; //��ǰ������ߴ�
		U* m_pUnits; //���ݴ洢��ָ��
};

#if defined(__WINDOWS__)
#define		tvsnprintf		_vsnprintf
#define		tstricmp		_stricmp
#define		tsnprintf		_snprintf
#elif defined(__LINUX__)
#define		tvsnprintf		vsnprintf
#define		tstricmp		strcasecmp
#define		tsnprintf		snprintf
#endif


CHAR		Value2Ascii(CHAR in);
CHAR		Ascii2Value(CHAR in);

BOOL		Binary2String(const CHAR* pIn,UINT InLength,CHAR* pOut);
BOOL		DBStr2Binary(const CHAR* pIn,UINT InLength,CHAR* pOut,UINT OutLimit,UINT& OutLength);
BOOL		String2Binary(const CHAR* pIn,UINT InLength,CHAR* pOut,UINT OutLimit,UINT& OutLength);
BOOL		StrSafeCheck(const CHAR* pIn,UINT InLength);
BOOL		CommandSafeCheck(const CHAR* pIn,UINT InLength);

#define		CHECK_STRING_NORMAL		0
#define		CHECK_STRING_CHARNAME	1

CHAR*		ReplaceIllegalString( CHAR* strText, INT nLength, INT nLevel = CHECK_STRING_NORMAL );
BOOL		CheckIllegalString( const CHAR* strText, INT nLength, INT nLevel = CHECK_STRING_NORMAL );



VOID		DumpStack(const CHAR* type);



#endif