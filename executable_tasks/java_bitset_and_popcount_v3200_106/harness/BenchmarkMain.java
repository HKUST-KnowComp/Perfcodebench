import java.util.*;

public class BenchmarkMain {
  static long[][] makeWords(int n) {
    long[] left = new long[n];
    long[] right = new long[n];
    long state = 0xDEADBEEF12345678L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      left[i] = state ^ (state << 7);
      state = state * 2862933555777941757L + 3037000493L;
      right[i] = state ^ (state >>> 11);
    }
    return new long[][] {left, right};
  }

  static long expectedCount(long[] left, long[] right) {
    long total = 0;
    for (int i = 0; i < left.length; i++) {
      total += Long.bitCount(left[i] & right[i]);
    }
    return total;
  }

  public static void main(String[] args) {
    long[][] words = makeWords(182200);
    long[] left = words[0];
    long[] right = words[1];
    long expected = expectedCount(left, right);
    long start = System.nanoTime();
    long actual = Solution.run(left, right, 36);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
