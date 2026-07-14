import java.util.*;

public class BenchmarkMain {
  static int[] makeDeltas(int n) {
    int[] deltas = new int[n];
    long state = 0xCAFEBABE13572468L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      deltas[i] = (int) ((state >>> 24) & 1023) - 284;
    }
    return deltas;
  }

  static long expectedSum(int[] deltas) {
    long current = 0;
    long total = 0;
    for (int d : deltas) {
      current += d;
      total += current + 0x9E3779B1L;
    }
    return total;
  }

  public static void main(String[] args) {
    int[] deltas = makeDeltas(117200);
    long expected = expectedSum(deltas);
    long start = System.nanoTime();
    long actual = Solution.run(deltas, 24);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
