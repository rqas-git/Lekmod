/* Read cached peer metadata with a temporary Steam client pipe, no game registration. */
#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
 if(argc!=3)return 2;void *lib=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);if(!lib){fprintf(stderr,"%s\n",dlerror());return 3;}
 void *(*create)(const char*,int*)=dlsym(lib,"CreateInterface");if(!create)return 4;
 void *c=create("SteamClient017",NULL);if(!c)return 5;void **v=*(void***)c;
 int pipe=((int(*)(void*))v[0])(c);if(!pipe)return 6;
 int user=((int(*)(void*,int))v[2])(c,pipe);if(!user)return 7;
 void *f=((void*(*)(void*,int,int,const char*))v[8])(c,user,pipe,"SteamFriends015");if(!f)return 8;
 uint64_t info[4]={0};void **fv=*(void***)f;bool ok=((bool(*)(void*,uint64_t,void*))fv[8])(f,strtoull(argv[2],NULL,10),info);
 printf("{\"ok\":%s,\"gameID\":%llu,\"lobbyID\":\"%llu\"}\n",ok?"true":"false",(unsigned long long)info[0],(unsigned long long)info[2]);fflush(stdout);
 ((void(*)(void*,int,int))v[4])(c,pipe,user);((bool(*)(void*,int))v[1])(c,pipe);return 0;
}
