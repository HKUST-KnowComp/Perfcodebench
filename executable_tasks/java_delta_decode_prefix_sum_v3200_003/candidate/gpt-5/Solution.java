public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;

    final int[] arr = deltas;
    final int n = arr.length;

    long current = 0L;
    long sum = 0L; // sum of all prefix values

    int i = 0;
    final int limit = n & ~7; // process in chunks of 8
    while (i < limit) {
      current += arr[i];     sum += current;
      current += arr[i + 1]; sum += current;
      current += arr[i + 2]; sum += current;
      current += arr[i + 3]; sum += current;
      current += arr[i + 4]; sum += current;
      current += arr[i + 5]; sum += current;
      current += arr[i + 6]; sum += current;
      current += arr[i + 7]; sum += current;
      i += 8;
    }
    while (i < n) {
      current += arr[i++];
      sum += current;
    }

    final long c = 0x9E3779B1L;
    return sum + c * (long) n;
  }
}
