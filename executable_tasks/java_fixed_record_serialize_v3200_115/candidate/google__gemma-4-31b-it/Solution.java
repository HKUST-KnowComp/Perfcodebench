import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    // Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
    int recordSize = 12;
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      long currentHash = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        // Serialize and hash ID (4 bytes)
        currentHash ^= (id & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((id >>> 8) & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((id >>> 16) & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((id >>> 24) & 0xFFL);
        currentHash *= 1099511628211L;

        // Serialize and hash TS (4 bytes)
        currentHash ^= (t & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((t >>> 8) & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((t >>> 16) & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((t >>> 24) & 0xFFL);
        currentHash *= 1099511628211L;

        // Serialize and hash Level (2 bytes)
        currentHash ^= (lvl & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((lvl >>> 8) & 0xFFL);
        currentHash *= 1099511628211L;

        // Serialize and hash Len (2 bytes)
        currentHash ^= (ln & 0xFFL);
        currentHash *= 1099511628211L;
        currentHash ^= ((ln >>> 8) & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }
    return hash;
  }
}