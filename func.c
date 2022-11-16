#include <key.h>
#include <chainlist.c>
static void key_init(key *mk) {
	mk->K0 = mk->K0 ^ fk[0];
	mk->K1 = mk->K1 ^ fk[1];
	mk->K2 = mk->K2 ^ fk[2];
	mk->K3 = mk->K3 ^ fk[3];
} 
static void init(key * mk) {
	mk->K0= 0x01234567;
	mk->K1= 0x89ABCDEF;
	mk->K2= 0xFEDCBA98;
	mk->K3= 0x76543210;
}
static void rand_init(key *mk) {
	srand(time(0));
	mk->K0= rand() << 16 | rand();
	mk->K1= rand() << 16 | rand();
	mk->K2= rand() << 16 | rand();
	mk->K3= rand() << 16 | rand();
	sleep(1);
}
static void invert(key *data) {
	uint32_t temp;
	temp=data->K3;
	data->K3=data->K0;
	data->K0=temp;

	temp=data->K2;
	data->K2=data->K1;
	data->K1=temp;
}
static void eq(key src,key *dst) {
	dst->K0=src.K0;
	dst->K1=src.K1;
	dst->K2=src.K2;
	dst->K3=src.K3;
}
static void move(key *mv,uint32_t fill) {
	mv->K0=mv->K1;
	mv->K1=mv->K2;
	mv->K2=mv->K3;
	mv->K3=fill;
}
static void getrk(state * state_obj,int i) {
	state_obj->rk = state_obj->k.K0 ^ L2(tau(state_obj->k.K1 ^ state_obj->k.K2 ^ state_obj->k.K3 ^ ck[i]));
	//printf("ck[%d]:%08x\n",i,ck[i]);
}
static void F(state * state_obj) {
	state_obj->dataseg = state_obj->midmsg.K0 ^ L1(tau(state_obj->midmsg.K1 ^ state_obj->midmsg.K2 ^ state_obj->midmsg.K3 ^ state_obj->rk));
}
static void state_copy(state src,state * dst) {
	dst->dataseg=src.dataseg;
	dst->rk=src.rk;

	dst->midmsg.K0=src.midmsg.K0;
	dst->midmsg.K1=src.midmsg.K1;
	dst->midmsg.K2=src.midmsg.K2;
	dst->midmsg.K3=src.midmsg.K3;

	dst->k.K0=src.k.K0;
	dst->k.K1=src.k.K1;
	dst->k.K2=src.k.K2;
	dst->k.K3=src.k.K3;
}

//加解密主要函数
void encrypt(key data, key k , key *output) {
	//加密
	key_init(&k);
	state state_obj = {k,data,0u};
	for(int i=0;i<32;i++) {	
		getrk(&state_obj,i);
		move(&(state_obj.k),state_obj.rk);
		F(&state_obj);
		move(&(state_obj.midmsg),state_obj.dataseg);
		//printf("rk[%d]:%08X x[%d]:%08X\n",i,state_obj.rk,i+4,state_obj.dataseg);
	}
	invert(&(state_obj.midmsg));
	eq(state_obj.midmsg,output);
}
//解密
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
		state_obj.rk=rk[31-i];
		F(&state_obj);
		move(&(state_obj.midmsg),state_obj.dataseg);

	}
	invert(&(state_obj.midmsg));
	eq(state_obj.midmsg,output);
}

state * encrypttimes(key data,key k,key *output,int rounds) {

	//得到完成rounds后的状态,rounds为0时表示不进行加密，输出初始状态
	key_init(&k);
	state state_obj = {k,data,0u};
	assert(rounds >= 0 && rounds <32);
	for(int i=0;i<rounds;i++) {	
		getrk(&state_obj,i);
		move(&(state_obj.k),state_obj.rk);
		F(&state_obj);
		move(&(state_obj.midmsg),state_obj.dataseg);
		//printf("rk[%d]:%08X x[%d]:%08X\n",i,state_obj.rk,i+4,state_obj.dataseg);
	}
	state * ret = malloc(sizeof(state));
	state_copy(state_obj,ret);
	return ret;

}
// 指定攻击第几轮
void attack(int rounds) {
	if(rounds > 32 || rounds <= 10) {
		assert(0);
	}
	//初始化操作
	key k;
	key msg;
	rand_init(&msg);
	rand_init(&k);
	uint32_t rk_array[4];
	uint32_t pos=0;
	
	while(pos<4){
		int initflag=0;
		big *Ana[2];
		Ana[0]=big_init();
		Ana[1]=big_init();
		
		while(1) {
			state* normal = encrypttimes(msg,k,NULL,rounds-1+pos);
			state* abnormal = encrypttimes(msg,k,NULL,rounds-1+pos);
			//注入
			sleep(1);
			srand(time(0));
			uint32_t interrupt_content = rand()%256;
			abnormal->midmsg.K1 = interrupt_content << 24 | (abnormal->midmsg.K1 & 0x00FFFFFF);
			abnormal->midmsg.K1 = interrupt_content << 16 | (abnormal->midmsg.K1 & 0xFF00FFFF);
			abnormal->midmsg.K2 = interrupt_content << 8 | (abnormal->midmsg.K2 & 0xFFFF00FF);
			abnormal->midmsg.K3 = interrupt_content  | (abnormal->midmsg.K3 & 0xFFFFFF00);
			/*
			printf("normal:\nX0:%08X X1:%08X X2:%08X X3:%08X\n",normal->midmsg.K0,normal->midmsg.K1,normal->midmsg.K2,normal->midmsg.K3);
			printf("interrput:%08X\n",interrupt_content);
			printf("abnormal:\nX0:%08X X1:%08X X2:%08X X3:%08X\n",abnormal->midmsg.K0,abnormal->midmsg.K1,abnormal->midmsg.K2,abnormal->midmsg.K3);
			*/
			//计算差分,不注入X0
	
			uint32_t diff_X1 = abnormal->midmsg.K1 ^ normal->midmsg.K1;
			uint32_t diff_X2 = abnormal->midmsg.K2 ^ normal->midmsg.K2;
			uint32_t diff_X3 = abnormal->midmsg.K3 ^ normal->midmsg.K3;
			uint32_t indiff = diff_X1 ^ diff_X2 ^ diff_X3;
			getrk(normal,rounds-1+pos);
			F(normal);
			getrk(abnormal,rounds-1+pos);
			F(abnormal);
			uint32_t diff_X4 = abnormal->dataseg ^ normal->dataseg;
			uint32_t outdiff = invL1(diff_X4);
			/*
			printf("diff_X1:%08X\n",diff_X1);
			printf("diff_X2:%08X\n",diff_X2);
			printf("diff_X3:%08X\n",diff_X3);
			printf("diff_X4:%08X\n",diff_X4);
			printf("indiff:%08X outdiff:%08X\n",indiff,outdiff);
			*/
			//猜测rk[i]的值
			for(uint32_t i =0;i<4;i++) {
				for(uint32_t j=0;j<=0xFF;j++) {
			
					uint32_t SA = ( outdiff & (0xFF << (i*8)) ) >> (i*8);
					uint32_t xor_123 = ( (normal->midmsg.K1 ^ normal->midmsg.K2 ^ normal->midmsg.K3) & ( 0xFF << (i*8) ) ) >> (i*8);
					uint32_t SB = box( xor_123 ^ j );

					uint32_t indiff_byte = ( indiff & (0xFF << (i*8)) ) >> (i*8);
					uint32_t SC = box(xor_123 ^ j ^ indiff_byte);
					if(SA==(SB^SC)) {
						if(Ana[initflag]->list[i]==NULL) {
							Ana[initflag]->list[i]=small_init(j);
						}
						else
						small_add(Ana[initflag]->list[i],j);
					}
				}
			}
			//通过多组差分进行收敛
			if(initflag) {
				for(int i=0;i<4;i++) {
					small *cache = Ana[0]->list[i];
					while(cache!=NULL) {
						int isExist=small_lookup(cache->num,Ana[1]->list[i]);
						uint32_t num=cache->num;
						if(!isExist) {
							cache=cache->next;
							small_delete(&(Ana[0]->list[i]),num);
						}
						else
						cache=cache->next;
					}
				}
			}
			else {
				initflag=1;
			}
			int len1=small_count(Ana[0]->list[0]);
			int len2=small_count(Ana[0]->list[1]);
			int len3=small_count(Ana[0]->list[2]);
			int len4=small_count(Ana[0]->list[3]);
			printf("len1=%d,Ana[0]->list[0]:",len1);
			small_view(Ana[0]->list[0]);
			printf("len2=%d,Ana[0]->list[1]:",len2);
			small_view(Ana[0]->list[1]);
			printf("len3=%d,Ana[0]->list[2]:",len3);
			small_view(Ana[0]->list[2]);
			printf("len4=%d,Ana[0]->list[3]:",len4);
			small_view(Ana[0]->list[3]);
			if(len1 ==1 && len2==1 && len3==1 && len4==1) {

				uint32_t rk = (Ana[0]->list[0]->num) | ( (Ana[0]->list[1]->num) << 8 ) ;
				rk = rk | ( (Ana[0]->list[2]->num) << 16 ) | ( (Ana[0]->list[3]->num) << 24 );
				rk_array[pos] = rk;
				break;
			}
			else {
				free(abnormal);
				free(normal);
			}
		}
		big_free(Ana[0]);
		big_free(Ana[1]);
		pos++;
	}

	key guess;
	
	guess.K0=rk_array[0];
	guess.K1=rk_array[1];
	guess.K2=rk_array[2];
	guess.K3=rk_array[3];
	printf("rk:");
	for(int i=0;i<4;i++) {
		printf("%X ",rk_array[i]);
	}
	printf("\n");
	//此时,得到的是rounds,rounds+1,rounds+2,rounds+3时的rk
	for(int i=0;i<rounds+3;i++) {
		uint32_t rkup = guess.K3 ^ L2(tau(guess.K0 ^ guess.K1 ^ guess.K2 ^ ck[rounds+2-i]));
		guess.K3=guess.K2;
		guess.K2=guess.K1;
		guess.K1=guess.K0;
		guess.K0=rkup;
	}
	key_init(&guess);
	printf("guess:%08X%08X%08X%08X\n",guess.K0,guess.K1,guess.K2,guess.K3);
	printf("real:%08X%08X%08X%08X\n",k.K0,k.K1,k.K2,k.K3);

}

