public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    byte[] bits = new byte[left.length * 64];
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < left.length; i++) {
        long word = left[i] & right[i];
        int base = i * 64;
        for (int b = 0; b < 64; b++) {
          bits[base + b] = (byte) ((word >>> b) & 1L);
        }
      }
      total = 0;
      for (byte bit : bits) total += bit;
    }
    return total;
  }
}
