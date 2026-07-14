public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    if (iters <= 0) return 0;
    long hash = 1469598103934665603L;
    for (int i = 0; i < ids.length; i++) {
      int id = ids[i];
      hash ^= (id & 0xFFL); hash *= 1099511628211L;
      hash ^= ((id >>> 8) & 0xFFL); hash *= 1099511628211L;
      hash ^= ((id >>> 16) & 0xFFL); hash *= 1099511628211L;
      hash ^= ((id >>> 24) & 0xFFL); hash *= 1099511628211L;

      int t = ts[i];
      hash ^= (t & 0xFFL); hash *= 1099511628211L;
      hash ^= ((t >>> 8) & 0xFFL); hash *= 1099511628211L;
      hash ^= ((t >>> 16) & 0xFFL); hash *= 1099511628211L;
      hash ^= ((t >>> 24) & 0xFFL); hash *= 1099511628211L;

      int level = levels[i];
      hash ^= (level & 0xFFL); hash *= 1099511628211L;
      hash ^= ((level >>> 8) & 0xFFL); hash *= 1099511628211L;

      int len = lens[i];
      hash ^= (len & 0xFFL); hash *= 1099511628211L;
      hash ^= ((len >>> 8) & 0xFFL); hash *= 1099511628211L;
    }
    return hash;
  }
}
