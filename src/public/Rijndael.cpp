//#ifndef RIJNDAEL_CPP
//#define RIJNDAEL_CPP

#include "Rijndael.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRijndael::CRijndael()
{

}

CRijndael::~CRijndael()
{

}
#define LARGE_TABLES

namespace
{
u1byte  pow_tab[256];
u1byte  log_tab[256];
u1byte  sbx_tab[256];
u1byte  isb_tab[256];
u4byte  rco_tab[ 10];
u4byte  ft_tab[4][256];
u4byte  it_tab[4][256];

#ifdef  LARGE_TABLES
  u4byte  fl_tab[4][256];
  u4byte  il_tab[4][256];
#endif

u4byte  tab_gen = 0;

#define RAND(a,b) (((a = 36969 * (a & 65535) + (a >> 16)) << 16) + (b = 18000 * (b & 65535) + (b >> 16))  )

#define ff_mult(a,b)    (a && b ? pow_tab[(log_tab[a] + log_tab[b]) % 255] : 0)

#define f_rn(bo, bi, n, k)                          \
    bo[n] =  ft_tab[0][byte(bi[n],0)] ^             \
             ft_tab[1][byte(bi[(n + 1) & 3],1)] ^   \
             ft_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             ft_tab[3][byte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rn(bo, bi, n, k)                          \
    bo[n] =  it_tab[0][byte(bi[n],0)] ^             \
             it_tab[1][byte(bi[(n + 3) & 3],1)] ^   \
             it_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             it_tab[3][byte(bi[(n + 1) & 3],3)] ^ *(k + n)

#ifdef LARGE_TABLES

#define ls_box(x)                \
    ( fl_tab[0][byte(x, 0)] ^    \
      fl_tab[1][byte(x, 1)] ^    \
      fl_tab[2][byte(x, 2)] ^    \
      fl_tab[3][byte(x, 3)] )

#define f_rl(bo, bi, n, k)                          \
    bo[n] =  fl_tab[0][byte(bi[n],0)] ^             \
             fl_tab[1][byte(bi[(n + 1) & 3],1)] ^   \
             fl_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             fl_tab[3][byte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rl(bo, bi, n, k)                          \
    bo[n] =  il_tab[0][byte(bi[n],0)] ^             \
             il_tab[1][byte(bi[(n + 3) & 3],1)] ^   \
             il_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             il_tab[3][byte(bi[(n + 1) & 3],3)] ^ *(k + n)

#else

#define ls_box(x)                            \
    ((u4byte)sbx_tab[byte(x, 0)] <<  0) ^    \
    ((u4byte)sbx_tab[byte(x, 1)] <<  8) ^    \
    ((u4byte)sbx_tab[byte(x, 2)] << 16) ^    \
    ((u4byte)sbx_tab[byte(x, 3)] << 24)

#define f_rl(bo, bi, n, k)                                      \
    bo[n] = (u4byte)sbx_tab[byte(bi[n],0)] ^                    \
        rotl(((u4byte)sbx_tab[byte(bi[(n + 1) & 3],1)]),  8) ^  \
        rotl(((u4byte)sbx_tab[byte(bi[(n + 2) & 3],2)]), 16) ^  \
        rotl(((u4byte)sbx_tab[byte(bi[(n + 3) & 3],3)]), 24) ^ *(k + n)

#define i_rl(bo, bi, n, k)                                      \
    bo[n] = (u4byte)isb_tab[byte(bi[n],0)] ^                    \
        rotl(((u4byte)isb_tab[byte(bi[(n + 3) & 3],1)]),  8) ^  \
        rotl(((u4byte)isb_tab[byte(bi[(n + 2) & 3],2)]), 16) ^  \
        rotl(((u4byte)isb_tab[byte(bi[(n + 1) & 3],3)]), 24) ^ *(k + n)

#endif

void gen_tabs(void)
{   u4byte  i, t;
    u1byte  p, q;

    // log and power tables for GF(2**8) finite field with  
    // 0x011b as modular polynomial - the simplest prmitive 
    // root is 0x03, used here to generate the tables       

    for(i = 0,p = 1; i < 256; ++i)
    {
        pow_tab[i] = (u1byte)p; log_tab[p] = (u1byte)i;

        p = p ^ (p << 1) ^ (p & 0x80 ? 0x01b : 0);
    }

    log_tab[1] = 0; p = 1;

    for(i = 0; i < 10; ++i)
    {
        rco_tab[i] = p; 

        p = (p << 1) ^ (p & 0x80 ? 0x1b : 0);
    }

    for(i = 0; i < 256; ++i)
    {   
        p = (i ? pow_tab[255 - log_tab[i]] : 0); q = p; 
        q = (q >> 7) | (q << 1); p ^= q; 
        q = (q >> 7) | (q << 1); p ^= q; 
        q = (q >> 7) | (q << 1); p ^= q; 
        q = (q >> 7) | (q << 1); p ^= q ^ 0x63; 
        sbx_tab[i] = p; isb_tab[p] = (u1byte)i;
    }

    for(i = 0; i < 256; ++i)
    {
        p = sbx_tab[i]; 

#ifdef  LARGE_TABLES        
        
        t = p; fl_tab[0][i] = t;
        fl_tab[1][i] = rotl(t,  8);
        fl_tab[2][i] = rotl(t, 16);
        fl_tab[3][i] = rotl(t, 24);
#endif
        t = ((u4byte)ff_mult(2, p)) |
            ((u4byte)p <<  8) |
            ((u4byte)p << 16) |
            ((u4byte)ff_mult(3, p) << 24);
        
        ft_tab[0][i] = t;
        ft_tab[1][i] = rotl(t,  8);
        ft_tab[2][i] = rotl(t, 16);
        ft_tab[3][i] = rotl(t, 24);

        p = isb_tab[i]; 

#ifdef  LARGE_TABLES        
        
        t = p; il_tab[0][i] = t; 
        il_tab[1][i] = rotl(t,  8); 
        il_tab[2][i] = rotl(t, 16); 
        il_tab[3][i] = rotl(t, 24);
#endif 
        t = ((u4byte)ff_mult(14, p)) |
            ((u4byte)ff_mult( 9, p) <<  8) |
            ((u4byte)ff_mult(13, p) << 16) |
            ((u4byte)ff_mult(11, p) << 24);
        
        it_tab[0][i] = t; 
        it_tab[1][i] = rotl(t,  8); 
        it_tab[2][i] = rotl(t, 16); 
        it_tab[3][i] = rotl(t, 24); 
    }

    tab_gen = 1;
}

#define star_x(x) (((x) & 0x7f7f7f7f) << 1) ^ ((((x) & 0x80808080) >> 7) * 0x1b)

#define imix_col(y,x)       \
    u   = star_x(x);        \
    v   = star_x(u);        \
    w   = star_x(v);        \
    t   = w ^ (x);          \
   (y)  = u ^ v ^ w;        \
   (y) ^= rotr(u ^ t,  8) ^ \
          rotr(v ^ t, 16) ^ \
          rotr(t,24)

}	// end of anonymous namespace

char *CRijndael::GetName(void)
{
    return "CRijndael";
}

// initialise the key schedule from the user supplied key   

#define loop4(i)                                    \
{   t = ls_box(rotr(t,  8)) ^ rco_tab[i];           \
    t ^= e_key[4 * i];     e_key[4 * i + 4] = t;    \
    t ^= e_key[4 * i + 1]; e_key[4 * i + 5] = t;    \
    t ^= e_key[4 * i + 2]; e_key[4 * i + 6] = t;    \
    t ^= e_key[4 * i + 3]; e_key[4 * i + 7] = t;    \
}

#define loop6(i)                                    \
{   t = ls_box(rotr(t,  8)) ^ rco_tab[i];           \
    t ^= e_key[6 * i];     e_key[6 * i + 6] = t;    \
    t ^= e_key[6 * i + 1]; e_key[6 * i + 7] = t;    \
    t ^= e_key[6 * i + 2]; e_key[6 * i + 8] = t;    \
    t ^= e_key[6 * i + 3]; e_key[6 * i + 9] = t;    \
    t ^= e_key[6 * i + 4]; e_key[6 * i + 10] = t;   \
    t ^= e_key[6 * i + 5]; e_key[6 * i + 11] = t;   \
}

#define loop8(i)                                    \
{   t = ls_box(rotr(t,  8)) ^ rco_tab[i];           \
    t ^= e_key[8 * i];     e_key[8 * i + 8] = t;    \
    t ^= e_key[8 * i + 1]; e_key[8 * i + 9] = t;    \
    t ^= e_key[8 * i + 2]; e_key[8 * i + 10] = t;   \
    t ^= e_key[8 * i + 3]; e_key[8 * i + 11] = t;   \
    t  = e_key[8 * i + 4] ^ ls_box(t);              \
    e_key[8 * i + 12] = t;                          \
    t ^= e_key[8 * i + 5]; e_key[8 * i + 13] = t;   \
    t ^= e_key[8 * i + 6]; e_key[8 * i + 14] = t;   \
    t ^= e_key[8 * i + 7]; e_key[8 * i + 15] = t;   \
}

void CRijndael::SetKey(const u1byte in_key[], const u4byte key_len)
{   u4byte  i, t, u, v, w;

    if(!tab_gen)

        gen_tabs();

    k_len = (key_len + 31) / 32;

    e_key[0] = u4byte_in(in_key     ); 
	e_key[1] = u4byte_in(in_key +  4);
    e_key[2] = u4byte_in(in_key +  8); 
	e_key[3] = u4byte_in(in_key + 12);

    switch(k_len)
    {
        case 4: t = e_key[3];
                for(i = 0; i < 10; ++i) 
                    loop4(i);
                break;

        case 6: e_key[4] = u4byte_in(in_key + 16); t = e_key[5] = u4byte_in(in_key + 20);
                for(i = 0; i < 8; ++i) 
                    loop6(i);
                break;

        case 8: e_key[4] = u4byte_in(in_key + 16); e_key[5] = u4byte_in(in_key + 20);
                e_key[6] = u4byte_in(in_key + 24); t = e_key[7] = u4byte_in(in_key + 28);
                for(i = 0; i < 7; ++i) 
                    loop8(i);
                break;
    }

    d_key[0] = e_key[0]; d_key[1] = e_key[1];
    d_key[2] = e_key[2]; d_key[3] = e_key[3];

    for(i = 4; i < 4 * k_len + 24; ++i)
    {
        imix_col(d_key[i], e_key[i]);
    }

    return;
}

// encrypt a block of text  

#define f_nround(bo, bi, k) \
    f_rn(bo, bi, 0, k);     \
    f_rn(bo, bi, 1, k);     \
    f_rn(bo, bi, 2, k);     \
    f_rn(bo, bi, 3, k);     \
    k += 4

#define f_lround(bo, bi, k) \
    f_rl(bo, bi, 0, k);     \
    f_rl(bo, bi, 1, k);     \
    f_rl(bo, bi, 2, k);     \
    f_rl(bo, bi, 3, k)

void CRijndael::Encrypt(const u1byte in_blk[16], u1byte out_blk[16])
{   u4byte  b0[4], b1[4], *kp;

    b0[0] = u4byte_in(in_blk    ) ^ e_key[0]; b0[1] = u4byte_in(in_blk +  4) ^ e_key[1];
    b0[2] = u4byte_in(in_blk + 8) ^ e_key[2]; b0[3] = u4byte_in(in_blk + 12) ^ e_key[3];

    kp = e_key + 4;

    if(k_len > 6)
    {
        f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    }

    if(k_len > 4)
    {
        f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    }

    f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    f_nround(b1, b0, kp); f_nround(b0, b1, kp);
    f_nround(b1, b0, kp); f_lround(b0, b1, kp);

    u4byte_out(out_blk,      b0[0]); u4byte_out(out_blk +  4, b0[1]);
    u4byte_out(out_blk +  8, b0[2]); u4byte_out(out_blk + 12, b0[3]);
}

// Decrypt a block of text  

#define i_nround(bo, bi, k) \
    i_rn(bo, bi, 0, k);     \
    i_rn(bo, bi, 1, k);     \
    i_rn(bo, bi, 2, k);     \
    i_rn(bo, bi, 3, k);     \
    k -= 4

#define i_lround(bo, bi, k) \
    i_rl(bo, bi, 0, k);     \
    i_rl(bo, bi, 1, k);     \
    i_rl(bo, bi, 2, k);     \
    i_rl(bo, bi, 3, k)

void CRijndael::Decrypt(const u1byte in_blk[16], u1byte out_blk[16])
{   u4byte  b0[4], b1[4], *kp;

    b0[0] = u4byte_in(in_blk     ) ^ e_key[4 * k_len + 24]; 
	b0[1] = u4byte_in(in_blk +  4) ^ e_key[4 * k_len + 25];
    b0[2] = u4byte_in(in_blk +  8) ^ e_key[4 * k_len + 26]; 
	b0[3] = u4byte_in(in_blk + 12) ^ e_key[4 * k_len + 27];

    kp = d_key + 4 * (k_len + 5);

    if(k_len > 6)
    {
        i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    }

    if(k_len > 4)
    {
        i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    }

    i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    i_nround(b1, b0, kp); i_nround(b0, b1, kp);
    i_nround(b1, b0, kp); i_lround(b0, b1, kp);

    u4byte_out(out_blk,     b0[0]); u4byte_out(out_blk +  4, b0[1]);
    u4byte_out(out_blk + 8, b0[2]); u4byte_out(out_blk + 12, b0[3]);
}

/************************************************************************/
/*AesEncrypt���ܺ���
/* In:   pInData	//Ҫ���ܵ����??
		 len        //��������ݵĳ���??
		 outLen		//�����ܺ����ݳ���
         pStrKey    //��Կ      
         keyLen     //��Կ����,Ĭ��Ϊ256λ	
/* Out:  ���ؼ��ܺ�����					                       */
/************************************************************************/
unsigned char* CRijndael::AesEncrypt(unsigned char* pInData,const int len,int& outLen,
						  unsigned char *pStrKey,const int keyLen)
{
	unsigned char* pEncData;
	unsigned char* pOutData;
		
	int length;
	int N;

	length = len;

	N = length / 16;

	if((length % 16) == 0)
	{
		//N=N;
	}
	else
	{
		length = length + 16 - (length % 16);
		N = N + 1;
	}

	pOutData = new unsigned char [length];
	pEncData = new unsigned char [length];	
	memcpy(pEncData,pInData,len);

	if((len % 16) != 0)
	{
		for(int i=0;i<(int)(16 - (len % 16));i++)
		{
			pEncData[length - 16 + (len % 16) + i]=' ';
		}
	}

	/************************************************************************/
	/* �������??......                                                       */
	/************************************************************************/	
	SetKey(pStrKey,keyLen);

	for(int j=0;j<N;j++)
	{
		Encrypt(pEncData,pOutData);
		pEncData += 16;
		pOutData += 16;
	}
	pEncData = pEncData - (16 * N);
	pOutData = pOutData - (16 * N);
		
	delete pEncData;
	pEncData = NULL;
	
	outLen = length;
	return pOutData;
}

/************************************************************************/
/*AesDecrypt���ܺ���
/* In:   pInData	//Ҫ���ܵ����??
		 len        //��������ݵĳ���??
         pStrKey    //��Կ      
         keyLen     //��Կ����,Ĭ��Ϊ256λ	
/* Out:  ���ؽ��ܺ�����ָ��					                       */
/************************************************************************/
unsigned char* CRijndael::AesDecrypt(unsigned char* pInData,const int len,unsigned char *pStrKey,
									 const int keyLen)
{	
	unsigned char* pDecData;
	unsigned char *pOutData;
	int length;
	int N;

	length = len;

	N = length / 16;

	pOutData = new unsigned char [length];
	pDecData = new unsigned char [length];
	memcpy(pDecData,pInData,len);
	
	/************************************************************************/
	/* �������??......                                                       */
	/************************************************************************/	
	SetKey(pStrKey,keyLen);

	for(int j=0;j<N;j++)
	{
		Decrypt(pDecData,pOutData);
		
		pOutData += 16;
		pDecData += 16;
	}
	pOutData = pOutData - (16 * N);
	pDecData = pDecData - (16 * N);

	delete pDecData;
	pDecData=NULL;

	return pOutData;
}

void CRijndael::FillRand(char *buf, const int len)
{  
	static unsigned long a[2];
	static unsigned long mt = 1;
	static unsigned long count = 4;
    static char          r[4];
    int                  i;

    if(mt) 
	{ 
		mt = 0;
		Cycles((boost::uint64_t *)a);
	}

    for(i = 0; i < len; ++i)
    {
        if(count == 4)
        {
            *(unsigned long*) r = RAND(a[0], a[1]);
            count = 0;
        }

        buf[i] = r[count++];
    }
}

//  A Pseudo Random Number Generator (PRNG) used for the
//  Initialisation Vector. The PRNG is George Marsaglia's
//  Multiply-With-Carry (MWC) PRNG that concatenates two
//  16-bit MWC generators:
//      x(n)=36969 * x(n-1) + carry mod 2^16
//      y(n)=18000 * y(n-1) + carry mod 2^16
//  to produce a combined PRNG with a period of about 2^60.
//  The Pentium cycle counter is used to initialise it. This
//  is crude but the IV does not really need to be secret.
void CRijndael::Cycles(volatile boost::uint64_t *rtn)
{
#if defined(_MSC_VER)
    __asm   // read the Pentium Time Stamp Counter
    {   cpuid
        rdtsc
        mov     ecx,rtn
        mov     [ecx],eax
        mov     [ecx+4],edx
        cpuid
    }
#else
	#include <time.h>
	time_t tt;
	tt     = time(NULL);

	((unsigned long*)rtn)[0] = tt;

	((unsigned long*)rtn)[1] = tt & -36969l;

	return;
#endif
}

//#endif
int CRijndael::Encrypt(const unsigned char* pInData, int iInDataLen, unsigned char* pOutData, const unsigned char* pKey, int iKeyLen)
{
	CRijndael theObj;

	unsigned char* pEncData;
	unsigned char* pTempData = pOutData;
		
	int length;
	int N;

	length = iInDataLen;

	N = length / 16;

	if((length % 16) == 0)
	{
		//N=N;
	}
	else
	{
		length = length + 16 - (length % 16);
		N = N + 1;
	}

	pEncData = new unsigned char[length];	
	memcpy( pEncData, pInData, iInDataLen );

	if((iInDataLen % 16) != 0)
	{
		for( int i = 0; i < (int)( 16 - (iInDataLen % 16) ); i++ )
		{
			pEncData[length - 16 + (iInDataLen % 16) + i]=' ';
		}
	}

	/************************************************************************/
	/* �������??......                                                       */
	/************************************************************************/	
	theObj.SetKey( pKey, iKeyLen );

	for( int j = 0; j < N; j++ )
	{
		theObj.Encrypt( pEncData, pTempData );
		pEncData += 16;
		pTempData += 16;
	}
	pEncData = pEncData - (16 * N);
	delete pEncData;
	
	return length;
}

void CRijndael::Decrypt(const unsigned char* pInData, int iInDataLen, unsigned char* pOutData, const unsigned char* pKey, int iKeyLen)
{
	CRijndael theObj;
	unsigned char* pDecData;
	unsigned char *pTempData = pOutData;
	int length;
	int N;

	length = iInDataLen;

	N = length / 16;

	pDecData = new unsigned char [length];
	memcpy( pDecData, pInData, iInDataLen);
	
	/************************************************************************/
	/* �������??......                                                       */
	/************************************************************************/	
	theObj.SetKey( pKey, iKeyLen );

	for(int j=0; j<N; j++)
	{
		theObj.Decrypt(pDecData,pTempData);
		
		pTempData += 16;
		pDecData += 16;
	}
	pDecData = pDecData - (16 * N);

	delete[] pDecData;
}
