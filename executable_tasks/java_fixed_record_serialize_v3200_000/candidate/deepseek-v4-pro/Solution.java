import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0;
    final long seed = 1469598103934665603L;
    final long mul = 1099511628211L;
    for (int iter = 0; iter < iters; iter++) {
      long h = seed;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        h ^= (id & 0xFF);
        h *= mul;
        h ^= ((id >>> 8) & 0xFF);
        h *= mul;
        h ^= ((id >>> 16) & 0xFF);
        h *= mul;
        h ^= ((id >>> 24) & 0xFF);
        h *= mul;

        int t = ts[i];
        h ^= (t & 0xFF);
        h *= mul;
        h ^= ((t >>> 8) & 0xFF);
        h *= mul;
        h ^= ((t >>> 16) & 0xFF);
        h *= mul;
        h ^= ((t >>> 24) & 0xFF);
        h *= mul;

        short lv = levels[i];
        h ^= (lv & 0xFF);
        h *= mul;
        h ^= ((lv >>> 8) & 0xFF);
        h *= mul;

        short ln = lens[i];
        h ^= (ln & 0xFF);
        h *= mul;
        h ^= ((ln >>> 8) & 0xFF);
        h *= mul;
      }
      hash = h;
    }
    return hash;
  }
}