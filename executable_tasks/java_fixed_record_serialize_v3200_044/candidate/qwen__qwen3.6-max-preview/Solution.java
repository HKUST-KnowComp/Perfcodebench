import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        h ^= (id & 0xFFL); h *= 1099511628211L;
        h ^= ((id >>> 8) & 0xFFL); h *= 1099511628211L;
        h ^= ((id >>> 16) & 0xFFL); h *= 1099511628211L;
        h ^= ((id >>> 24) & 0xFFL); h *= 1099511628211L;
        
        int t = ts[i];
        h ^= (t & 0xFFL); h *= 1099511628211L;
        h ^= ((t >>> 8) & 0xFFL); h *= 1099511628211L;
        h ^= ((t >>> 16) & 0xFFL); h *= 1099511628211L;
        h ^= ((t >>> 24) & 0xFFL); h *= 1099511628211L;
        
        int lvl = levels[i];
        h ^= (lvl & 0xFFL); h *= 1099511628211L;
        h ^= ((lvl >>> 8) & 0xFFL); h *= 1099511628211L;
        
        int ln = lens[i];
        h ^= (ln & 0xFFL); h *= 1099511628211L;
        h ^= ((ln >>> 8) & 0xFFL); h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}
