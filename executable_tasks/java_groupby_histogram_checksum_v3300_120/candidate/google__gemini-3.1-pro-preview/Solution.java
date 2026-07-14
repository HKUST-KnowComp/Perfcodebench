public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters <= 0) return 0;
    
    long[] hist = new long[buckets];
    int n = keys.length;
    
    int i = 0;
    for (; i <= n - 4; i += 4) {
      hist[keys[i]] += vals[i];
      hist[keys[i+1]] += vals[i+1];
      hist[keys[i+2]] += vals[i+2];
      hist[keys[i+3]] += vals[i+3];
    }
    for (; i < n; i++) {
      hist[keys[i]] += vals[i];
    }
    
    long h = 1469598103934665603L;
    for (int j = 0; j < buckets; j++) {
      h ^= (hist[j] + 1);
      h *= 1099511628211L;
    }
    
    return h;
  }
}
