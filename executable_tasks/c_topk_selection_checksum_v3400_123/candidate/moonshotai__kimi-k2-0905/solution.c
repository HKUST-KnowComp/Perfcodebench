#include <stdint.h>
#include <stdlib.h>

#define SWAP(a,b,T) do{ T tmp=a; a=b; b=tmp; }while(0)

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair(const void* a,const void* b){
  const Pair *pa=a, *pb=b;
  if(pa->score>pb->score) return -1;
  if(pa->score<pb->score) return 1;
  if(pa->index<pb->index) return -1;
  if(pa->index>pb->index) return 1;
  return 0;
}

/* inline Lomuto partition around pivot; returns final pivot index */
static size_t partition(Pair* a,size_t l,size_t r){
  Pair pivot=a[l]; size_t j=l+1;        /* choose leftmost as pivot */
  for(size_t i=l+1;i<=r;i++){
    if(a[i].score>pivot.score || (a[i].score==pivot.score && a[i].index<=pivot.index))
      SWAP(a[i],a[j++],Pair);
  }
  SWAP(a[l],a[j-1],Pair);
  return j-1;
}

/* quickselect returning k-th largest by score (stable partitioning) */
static size_t kth_largest_topk(Pair* a,size_t n,size_t k){
  size_t l=0,r=n-1;
  while(1){
    size_t i=partition(a,l,r);
    if(i==k) return i;
    if(i<k) l=i+1; else r=i-1;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters){
  Pair* pairs=(Pair*)malloc(n*sizeof(Pair));
  if(!pairs) return 0;

  uint64_t h = 1469598103934665603ULL; /* FNV init */
  for(int iter=0;iter<iters;iter++){
    /* fill and then inline init quickselect partition to rank k+1 */
    for(size_t i=0;i<n;i++){ pairs[i].score=scores[i]; pairs[i].index=(uint32_t)i; }

    /* locate the score threshold and arrange top k to front */
    kth_largest_topk(pairs,n,k);

    /* sort only the k winners by the original descending rule */
    if((size_t)k>1) qsort(pairs,(size_t)k,sizeof(Pair),cmp_pair);

    /* compute rolling checksum */
    for(int i=0;i<k;i++){
      h ^= ((uint64_t)pairs[i].score<<32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}