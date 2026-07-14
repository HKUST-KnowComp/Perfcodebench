#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef N
#define N 256
#endif

static __thread uint32_t scratch_buf[N]={0};
static __thread size_t scratch_size=0;

extern size_t __dict_size;
static __attribute__((constructor,unused)) void init_buf(void){
    const char* e=getenv("MAX_DICT_SIZE");
    size_t want=e?(size_t)atoi(e):N;
    if(want>N-1) want=N-1;
    scratch_size=want;
}

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size) {
    /* if dict_size <= dict_size, copy into our private buffer and run; else fallback to temporary alloc */
    uint32_t* buf=NULL;
    if(dict_size<=scratch_size){ buf=scratch_buf; }
    else{ buf=(uint32_t*)malloc(dict_size*sizeof(uint32_t)); if(!buf) return 0; }

    for(uint32_t i=0;i<dict_size;i++) buf[i]=dict_vals[i];

    const uint64_t prime=1099511628211ULL;
    const uint64_t offset=1469598103934665603ULL;
    uint64_t h=offset;

    /* unroll processing 8 lookups at a time */
    size_t i=0;
    for(;i+8<=n;i+=8){
        uint32_t v0=buf[ids[i+0]];
        uint32_t v1=buf[ids[i+1]];  h^=(v0+1); h*=prime;
        uint32_t v2=buf[ids[i+2]];  h^=(v1+1); h*=prime;
        uint32_t v3=buf[ids[i+3]];  h^=(v2+1); h*=prime;
        uint32_t v4=buf[ids[i+4]];  h^=(v3+1); h*=prime;
        uint32_t v5=buf[ids[i+5]];  h^=(v4+1); h*=prime;
        uint32_t v6=buf[ids[i+6]];  h^=(v5+1); h*=prime;
        uint32_t v7=buf[ids[i+7]];  h^=(v6+1); h*=prime;
                                  h^=(v7+1); h*=prime;
    }
    for(;i<n;i++){
        uint32_t v=buf[ids[i]];
        h^=(v+1);
        h*=prime;
    }

    if(buf!=scratch_buf) free(buf);
    return h;
}