public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long finalHash = 0;
    int n = ids.length;
    long mul = 1099511628211L;
    long init = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = init;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        hash ^= (id & 0xFFL);
        hash *= mul;
        hash ^= ((id >>> 8) & 0xFFL);
        hash *= mul;
        hash ^= ((id >>> 16) & 0xFFL);
        hash *= mul;
        hash ^= ((id >>> 24) & 0xFFL);
        hash *= mul;

        int t = ts[i];
        hash ^= (t & 0xFFL);
        hash *= mul;
        hash ^= ((t >>> 8) & 0xFFL);
        hash *= mul;
        hash ^= ((t >>> 16) & 0xFFL);
        hash *= mul;
        hash ^= ((t >>> 24) & 0xFFL);
        hash *= mul;

        short lev = levels[i];
        hash ^= (lev & 0xFFL);
        hash *= mul;
        hash ^= ((lev >>> 8) & 0xFFL);
        hash *= mul;

        short len = lens[i];
        hash ^= (len & 0xFFL);
        hash *= mul;
        hash ^= ((len >>> 8) & 0xFFL);
        hash *= mul;
      }
      finalHash = hash;
    }
    return finalHash;
  }
}
