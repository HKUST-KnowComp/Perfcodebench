import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    int len = keys.length;
    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0);
      int i = 0;
      int limit = len - 4;
      for (; i <= limit; i += 4) {
        hist[keys[i]]   += vals[i];
        hist[keys[i+1]] += vals[i+1];
        hist[keys[i+2]] += vals[i+2];
        hist[keys[i+3]] += vals[i+3];
      }
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      
      h = 1469598103934665603L;
      for (int j = 0; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
