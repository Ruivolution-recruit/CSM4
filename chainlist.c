#include <stdint.h>
#include <stdlib.h>
typedef struct small {
	uint32_t num;
	struct small * next;
}small;

typedef struct big {
	small* list[4];
	struct big *next;
}big;

big * big_init () {
	big * ret =malloc(sizeof(big));
	for(int i=0;i<4;i++)
		ret->list[i]=NULL;
	return ret;
}

small * small_init(uint32_t num) {
	small * ret = malloc(sizeof(small));
	ret->num=num;
	ret->next=NULL;
	return ret;
}
void small_view(small *head) {
	if(head==NULL) {
		printf("empty\n");
		return;
	}
	small * cache =head;
	while(cache!=NULL) {
		printf("%08x ",cache->num);
		cache=cache->next;
	}
	printf("\n");
	return;
}
void small_add(small * head,uint32_t num) {
	if(head==NULL)
		return;
	small * cache=head;
	while(cache->next != NULL) {
		cache=cache->next;
	}
	small * new = malloc(sizeof(small));
	new ->num =num;
	new->next =NULL;
	cache->next=new;
}
int small_lookup(uint32_t t,small * head) {
	if(head==NULL)
		return 0;
	small * cache = head;
	while(cache!=NULL) {
		if(t==cache->num)
			return 1;
		cache=cache->next;
	}
	return 0;
}
void small_delete(small ** head,uint32_t t) {
	if(*head==NULL)
		return;
	small * front= *head;
	small * cache= *head;
	if(cache->num==t) {
		*head=cache->next;
		return;
	}
	while(cache!=NULL) {
		if(cache->num==t) {
			front->next=cache->next;
			free(cache);
			return;
		}
		front=cache;
		cache=cache->next;
	}
}
int small_count(small *head) {
	int ret=0;
	small * cache = head;
	while(cache!=NULL) {
		ret++;
		cache=cache->next;
	}
	return ret;
}
void small_free(small * head) {
	if(head==NULL) {
		printf("empty\n");
		return ;
	}
	small *keep=head->next;
	small *now=head;
	while(keep!=NULL) {
		free(now);
		now=keep;
		keep=keep->next;
	}
	free(now);
}
void big_free(big * head) {
	if(head==NULL)
		return;
	for(int i=0;i<4;i++) {
		small_free(head->list[i]);
	}
	free(head);
}