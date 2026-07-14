public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    int len = keys.length;
    int i = 0;
    int limit = len - 3;
    for (; i < limit; i += 4) {
      hist[keys[i]] += vals[i];
      hist[keys[i+1]] += vals[i+1];
      hist[keys[i+2]] += vals[i+2];
      hist[keys[i+3]] += vals[i+3];
    }
    for (; i < len; i++) {
      hist[keys[i]] += vals[i];
    }
    long h = 1469598103934665603L;
    for (long v : hist) {
      h ^= (v + 1);
      h *= 1099511628211L;
    }
    return h;
  }
}