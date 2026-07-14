public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters == 0) return 0L;
    final int[] idx = ids;
    final int[] dict = dictVals;
    final int len = idx.length;

    long h = 1469598103934665603L;
    final long mul = 1099511628211L;

    int i = 0;
    int limit = len & ~3; // process 4 at a time
    while (i < limit) {
      int v0 = dict[idx[i    ]];
      int v1 = dict[idx[i + 1]];
      int v2 = dict[idx[i + 2]];
      int v3 = dict[idx[i + 3]];
      h ^= ((long) v0 & 0xFFFFFFFFL) + 1L; h *= mul;
      h ^= ((long) v1 & 0xFFFFFFFFL) + 1L; h *= mul;
      h ^= ((long) v2 & 0xFFFFFFFFL) + 1L; h *= mul;
      h ^= ((long) v3 & 0xFFFFFFFFL) + 1L; h *= mul;
      i += 4;
    }
    while (i < len) {
      int v = dict[idx[i++]];
      h ^= ((long) v & 0xFFFFFFFFL) + 1L;
      h *= mul;
    }
    return h;
  }
}
