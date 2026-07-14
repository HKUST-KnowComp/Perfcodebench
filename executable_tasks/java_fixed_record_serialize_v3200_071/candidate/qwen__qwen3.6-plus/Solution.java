import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0;
    final long FNV_PRIME = 1099511628211L;
    final int n = ids.length;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        hash ^= (id & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((id >>> 8) & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((id >>> 16) & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((id >>> 24) & 0xFFL); hash *= FNV_PRIME;

        int t = ts[i];
        hash ^= (t & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((t >>> 8) & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((t >>> 16) & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((t >>> 24) & 0xFFL); hash *= FNV_PRIME;

        short l = levels[i];
        hash ^= (l & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((l >>> 8) & 0xFFL); hash *= FNV_PRIME;

        short le = lens[i];
        hash ^= (le & 0xFFL); hash *= FNV_PRIME;
        hash ^= ((le >>> 8) & 0xFFL); hash *= FNV_PRIME;
      }
    }
    return hash;
  }
}