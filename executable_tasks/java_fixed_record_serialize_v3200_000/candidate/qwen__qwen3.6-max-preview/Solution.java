public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0;
    int n = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        h = (h ^ (id & 0xFFL)) * 1099511628211L;
        h = (h ^ ((id >>> 8) & 0xFFL)) * 1099511628211L;
        h = (h ^ ((id >>> 16) & 0xFFL)) * 1099511628211L;
        h = (h ^ ((id >>> 24) & 0xFFL)) * 1099511628211L;
        
        int t = ts[i];
        h = (h ^ (t & 0xFFL)) * 1099511628211L;
        h = (h ^ ((t >>> 8) & 0xFFL)) * 1099511628211L;
        h = (h ^ ((t >>> 16) & 0xFFL)) * 1099511628211L;
        h = (h ^ ((t >>> 24) & 0xFFL)) * 1099511628211L;
        
        int lv = levels[i] & 0xFFFF;
        h = (h ^ (lv & 0xFFL)) * 1099511628211L;
        h = (h ^ ((lv >>> 8) & 0xFFL)) * 1099511628211L;
        
        int ln = lens[i] & 0xFFFF;
        h = (h ^ (ln & 0xFFL)) * 1099511628211L;
        h = (h ^ ((ln >>> 8) & 0xFFL)) * 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}