import java.util.*;

public class BenchmarkMain {
  static int[] makeScores(int n) {
    int[] scores = new int[n];
    long state = 0x3141592653589793L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      scores[i] = (int) ((state >>> 16) & 0xFFFFF);
    }
    return scores;
  }

  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  static long checksumTopK(int[] scores, int k) {
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) pairs[i] = new Pair(scores[i], i);
    Arrays.sort(pairs, (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    });
    long h = 1469598103934665603L;
    for (int i = 0; i < k; i++) {
      h ^= (((long) pairs[i].score) << 32) ^ (pairs[i].index & 0xffffffffL);
      h *= 1099511628211L;
    }
    return h;
  }

  public static void main(String[] args) {
    int[] scores = makeScores(57900);
    long expected = checksumTopK(scores, 12);
    long start = System.nanoTime();
    long actual = Solution.run(scores, 12, 33);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
