import java.util.*;

public class BenchmarkMain {
  static Object[] makeInput(int n, int buckets) {
    int[] keys = new int[n];
    int[] vals = new int[n];
    long state = 0xA55A5AA55A5AA55AL;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      keys[i] = (int) ((state >>> 16) % buckets);
      vals[i] = 1 + (int) ((state >>> 40) & 31);
    }
    return new Object[] {keys, vals};
  }

  static long expectedChecksum(int[] keys, int[] vals, int buckets) {
    long[] hist = new long[buckets];
    for (int i = 0; i < keys.length; i++) hist[keys[i]] += vals[i];
    long h = 1469598103934665603L;
    for (long v : hist) {
      h ^= (v + 1);
      h *= 1099511628211L;
    }
    return h;
  }

  public static void main(String[] args) {
    Object[] input = makeInput(429100, 192);
    int[] keys = (int[]) input[0];
    int[] vals = (int[]) input[1];
    long expected = expectedChecksum(keys, vals, 192);
    long start = System.nanoTime();
    long actual = Solution.run(keys, vals, 192, 29);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
