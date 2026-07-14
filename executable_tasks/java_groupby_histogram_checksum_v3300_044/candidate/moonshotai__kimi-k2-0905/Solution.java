import jdk.incubator.vector.*;

public class Solution {
  private static final VectorSpecies<Integer> I256 = IntVector.SPECIES_256;
  private static final VectorSpecies<Long> L256 = LongVector.SPECIES_256;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    // FNV-1a constants
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME  = 1099511628211L;

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];

      // Vectorized histogram update
      int i = 0;
      int upper = keys.length - (keys.length % I256.length());
      for (; i < upper; i += I256.length()) {
        IntVector k = IntVector.fromArray(I256, keys, i);
        IntVector v = IntVector.fromArray(I256, vals, i);
        for (int j = 0; j < I256.length(); j++) {
          int bucket = k.lane(j);
          hist[bucket] += (long) v.lane(j);
        }
      }
      // Tail scalar
      for (; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }

      // Unrolled checksum
      h = FNV_OFFSET;
      int limit = hist.length & ~7;
      int k = 0;
      for (; k < limit; k += 8) {
        h ^= (hist[k]   + 1); h *= FNV_PRIME;
        h ^= (hist[k+1] + 1); h *= FNV_PRIME;
        h ^= (hist[k+2] + 1); h *= FNV_PRIME;
        h ^= (hist[k+3] + 1); h *= FNV_PRIME;
        h ^= (hist[k+4] + 1); h *= FNV_PRIME;
        h ^= (hist[k+5] + 1); h *= FNV_PRIME;
        h ^= (hist[k+6] + 1); h *= FNV_PRIME;
        h ^= (hist[k+7] + 1); h *= FNV_PRIME;
      }
      for (; k < hist.length; k++) {
        h ^= (hist[k] + 1);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}