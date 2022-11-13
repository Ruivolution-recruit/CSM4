#include <key.h>
//State:{K,midmsg,rk}



static void key_init(key *mk) {
	mk->K0 = mk->K0 ^ fk[0];
	mk->K1 = mk->K1 ^ fk[1];
	mk->K2 = mk->K2 ^ fk[2];
	mk->K3 = mk->K3 ^ fk[3];
} 
static void init(key * mk) {
#ifdef ENABLE_RAND
	srand(time(0));
	mk->K0= rand() << 16 | rand();
	mk->K1= rand() << 16 | rand();
	mk->K2= rand() << 16 | rand();
	mk->K3= rand() << 16 | rand();
	sleep(1);
#else
	mk->K0= 0x01010101;
	mk->K1= 0x01010101;
	mk->K2= 0x01010101;
	mk->K3= 0x01010101;
#endif
}

void invert(key *data) {
	uint32_t temp;
	temp=data->K3;
	data->K3=data->K0;
	data->K0=temp;

	temp=data->K2;
	data->K2=data->K1;
	data->K1=temp;
}
void eq(key src,key *dst) {
	dst->K0=src.K0;
	dst->K1=src.K1;
	dst->K2=src.K2;
	dst->K3=src.K3;
}
void encrypt(key data, key k , key *output) {
	key_init(&k);
	state state_obj = {k,data,0u};
	for(int i=0;i<32;i++) {
		state_obj.rk = state_obj.k.K0 ^ L2(tau(state_obj.k.K1 ^ state_obj.k.K2 ^ state_obj.k.K3 ^ ck[i]));
		
		state_obj.k.K0=state_obj.k.K1;
		state_obj.k.K1=state_obj.k.K2;
		state_obj.k.K2=state_obj.k.K3;
		state_obj.k.K3=state_obj.rk;

		state_obj.dataseg = state_obj.midmsg.K0 ^ L1(tau(state_obj.midmsg.K1 ^ state_obj.midmsg.K2 ^ state_obj.midmsg.K3 ^ state_obj.rk));
		state_obj.midmsg.K0=state_obj.midmsg.K1;
		state_obj.midmsg.K1=state_obj.midmsg.K2;
		state_obj.midmsg.K2=state_obj.midmsg.K3;
		state_obj.midmsg.K3=state_obj.dataseg;

	}
	invert(&(state_obj.midmsg));
	eq(state_obj.midmsg,output);
}


void decrypt(key edata,key k,key *output) {
	state state_obj ={k,edata,0u};
	uint32_t rk[32]={};
	key_init(&(state_obj.k));
	for(int i=0;i<32;i++) {
		state_obj.rk = state_obj.k.K0 ^ L2(tau(state_obj.k.K1 ^ state_obj.k.K2 ^ state_obj.k.K3 ^ ck[i]));

		state_obj.k.K0=state_obj.k.K1;
		state_obj.k.K1=state_obj.k.K2;
		state_obj.k.K2=state_obj.k.K3;
		state_obj.k.K3=state_obj.rk;
		rk[i]=state_obj.rk;
	}

	for(int i=0;i<32;i++) {
		state_obj.dataseg = state_obj.midmsg.K0 ^ L1(tau(state_obj.midmsg.K1 ^ state_obj.midmsg.K2 ^ state_obj.midmsg.K3 ^ rk[31-i]));

		state_obj.midmsg.K0=state_obj.midmsg.K1;
		state_obj.midmsg.K1=state_obj.midmsg.K2;
		state_obj.midmsg.K2=state_obj.midmsg.K3;
		state_obj.midmsg.K3=state_obj.dataseg;
	}
	invert(&(state_obj.midmsg));
	eq(state_obj.midmsg,output);
}