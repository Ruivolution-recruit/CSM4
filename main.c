#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inc.h>

key k;
key data;
key output;

int main() {
	init(&k); //key init
	init(&data);
	printf("result:\n");
	printf("key:    %08X%08X%08X%08X\ndata:   %08X%08X%08X%08X\n",k.K0,k.K1,k.K2,k.K3,data.K0,data.K1,data.K2,data.K3);
	encrypt(data,k,&output);
	printf("encrypt:%08X%08X%08X%08X\n",output.K0,output.K1,output.K2,output.K3);
	decrypt(output,k,&output);
	assert(data.K0 == output.K0 );
	assert(data.K1 == output.K1 );
	assert(data.K2 == output.K2 );
	assert(data.K3 == output.K3 );
	printf("decrypt:%08X%08X%08X%08X\n",output.K0,output.K1,output.K2,output.K3);
	
	return 0;
}