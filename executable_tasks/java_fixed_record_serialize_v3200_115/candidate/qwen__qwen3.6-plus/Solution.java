public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0;
    final long FNV_PRIME = 1099511628211L;
    final long FNV_OFFSET = 1469598103934665603L;
    int n = ids.length;
    
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        h ^= (id & 0xFFL); h *= FNV_PRIME;
        h ^= ((id >>> 8) & 0xFFL); h *= FNV_PRIME;
        h ^= ((id >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((id >>> 24) & 0xFFL); h *= FNV_PRIME;
        
        int t = ts[i];
        h ^= (t & 0xFFL); h *= FNV_PRIME;
        h ^= ((t >>> 8) & 0xFFL); h *= FNV_PRIME;
        h ^= ((t >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((t >>> 24) & 0xFFL); h *= FNV_PRIME;
        
        short lv = levels[i];
        h ^= (lv & 0xFFL); h *= FNV_PRIME;
        h ^= ((lv >>> 8) & 0xFFL); h *= FNV_PRIME;
        
        short ln = lens[i];
        h ^= (ln & 0xFFL); h *= FNV_PRIME;
        h ^= ((ln >>> 8) & 0xFFL); h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}