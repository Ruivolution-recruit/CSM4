#include <stdint.h>
#include <sbox.h>
#include <time.h>
#include <unistd.h>

// define the marco to make calculation convenient,all for uint32_t
//basic cir cal
#define cr(x,y) ((x >> y) | (x << (32 - y)))
#define cl(x,y) ((x << y) | (x >> (32 - y)))
//encrypt
static uint32_t tau(uint32_t t) {
	uint32_t t1=box( (t & 0xFF000000 ) >> 24 ) << 24;
	uint32_t t2=box( (t & 0x00FF0000 ) >> 16 ) << 16;
	uint32_t t3=box( (t & 0x0000FF00 ) >> 8  ) << 8 ;
	uint32_t t4=box( (t & 0x000000FF ) );
	return t1 | t2 | t3 | t4;
}
#define L1(x) (x ^ cl(x,2) ^ cl(x,10) ^ cl(x,18) ^ cl(x,24)) // encrypt liner trans
//key 
#define L2(x) (x ^ cl(x,13) ^ cl(x,23))
#define invL1(x) ((x) ^ cl((x),2) ^ cl((x),4) ^ cl((x),8) ^ cl((x),12) ^ cl((x),14) ^ cl((x),16) ^ cl((x),18) ^ cl((x),22) ^ cl((x),24) ^ cl((x),30))

static uint32_t fk[] = {0xA3B1BAC6,0x56AA3350,0x677D9197,0xB27022DC};
static uint32_t ck[] = {0X00070E15,0X1C232A31,0X383F464D,0X545B6269,0X70777E85,0X8C939AA1,0XA8AFB6BD,0XC4CBD2D9,0XE0E7EEF5,0XFC030A11,0X181F262D,0X343B4249,0X50575E65,0X6C737A81,0X888F969D,0XA4ABB2B9,0XC0C7CED5,0XDCE3EAF1,0XF8FF060D,0X141B2229,0X30373E45,0X4C535A61,0X686F767D,0X848B9299,0XA0A7AEB5,0XBCC3CAD1,0XD8DFE6ED,0XF4FB0209,0X10171E25,0X2C333A41,0X484F565D,0X646B7279};


typedef struct key
{
	uint32_t K0;
	uint32_t K1;
	uint32_t K2;
	uint32_t K3;
}key;

typedef struct {
	key k;
	key midmsg;
	uint32_t rk;
	uint32_t dataseg;
}state;



