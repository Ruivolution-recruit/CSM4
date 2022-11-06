#include <stdio.h>
#include <stdlib.h>

#include <inc.h>

key k;
key data;

int main() {
	init(&k); //key init
	init(&data);
	printf("key:%X%X%X%X\ndata:%X%X%X%X\n",k.K0,k.K1,k.K2,k.K3,data.K0,data.K1,data.K2,data.K3);
	encrypt(&data,k);
	decrypt(&data,k);
	printf("encrypt:%X%X%X%X\n",data.K0,data.K1,data.K2,data.K3);
	
	return 0;
}