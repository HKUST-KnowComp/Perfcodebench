public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 1469598103934665603L;
    long prime = 1099511628211L;
    int n = ids.length;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lv = levels[i];
        short ln = lens[i];
        
        hash ^= (id & 0xFFL); hash *= prime;
        hash ^= ((id >>> 8) & 0xFFL); hash *= prime;
        hash ^= ((id >>> 16) & 0xFFL); hash *= prime;
        hash ^= ((id >>> 24) & 0xFFL); hash *= prime;
        
        hash ^= (t & 0xFFL); hash *= prime;
        hash ^= ((t >>> 8) & 0xFFL); hash *= prime;
        hash ^= ((t >>> 16) & 0xFFL); hash *= prime;
        hash ^= ((t >>> 24) & 0xFFL); hash *= prime;
        
        hash ^= (lv & 0xFFL); hash *= prime;
        hash ^= ((lv >>> 8) & 0xFFL); hash *= prime;
        
        hash ^= (ln & 0xFFL); hash *= prime;
        hash ^= ((ln >>> 8) & 0xFFL); hash *= prime;
      }
    }
    return hash;
  }
}