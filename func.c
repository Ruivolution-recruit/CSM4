#include <key.h>
void invert(key *data) {
	uint32_t temp;
	temp=data->K3;
	data->K3=data->K0;
	data->K0=temp;

	temp=data->K2;
	data->K2=data->K1;
	data->K1=temp;
}
void encrypt(key *data, key k) {
	key ktemp={k.K0,k.K1,k.K2,k.K3};
	key_init(&ktemp);
	for(int i=0;i<32;i++) {
		uint32_t Knew = ktemp.K0 ^ L2(tau(ktemp.K1 ^ ktemp.K2 ^ ktemp.K3 ^ ck[i]));
		
		ktemp.K0=ktemp.K1;
		ktemp.K1=ktemp.K2;
		ktemp.K2=ktemp.K3;
		ktemp.K3=Knew;

		uint32_t Dnew = data->K0 ^ L1(tau(data->K1 ^ data->K2 ^ data->K3 ^ Knew));

		data->K0=data->K1;
		data->K1=data->K2;
		data->K2=data->K3;
		data->K3=Dnew;
	}
	invert(data);
}

void decrypt(key *edata,key k) {
	key ktemp={k.K0,k.K1,k.K2,k.K3};

	uint32_t rk[32]={};
	key_init(&ktemp);
	for(int i=0;i<32;i++) {
		uint32_t Knew = ktemp.K0 ^ L2(tau(ktemp.K1 ^ ktemp.K2 ^ ktemp.K3 ^ ck[i]));

		ktemp.K0=ktemp.K1;
		ktemp.K1=ktemp.K2;
		ktemp.K2=ktemp.K3;
		ktemp.K3=Knew;
		rk[i]=Knew;
	}

	for(int i=0;i<32;i++) {
		uint32_t Dnew = edata->K0 ^ L1(tau(edata->K1 ^ edata->K2 ^ edata->K3 ^ rk[31-i]));

		edata->K0=edata->K1;
		edata->K1=edata->K2;
		edata->K2=edata->K3;
		edata->K3=Dnew;
	}
	invert(edata);
}