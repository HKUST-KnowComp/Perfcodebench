public class BenchmarkMain {
  static Object[] makeInput(int n, int dictSize) {
    int[] ids = new int[n];
    int[] dictVals = new int[dictSize];
    long state = 0x9E3779B97F4A7C15L;
    for (int i = 0; i < dictSize; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      dictVals[i] = (int) ((state >>> 16) & 0xFFFFFF);
    }
    for (int i = 0; i < n; i++) {
      state = state * 2862933555777941757L + 3037000493L;
      ids[i] = (int) ((state >>> 20) % dictSize);
    }
    return new Object[] {ids, dictVals};
  }

  static long expectedChecksum(int[] ids, int[] dictVals) {
    long h = 1469598103934665603L;
    for (int id : ids) {
      h ^= (dictVals[id] & 0xffffffffL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }

  public static void main(String[] args) {
    Object[] input = makeInput(493400, 1024);
    int[] ids = (int[]) input[0];
    int[] dictVals = (int[]) input[1];
    long expected = expectedChecksum(ids, dictVals);
    long start = System.nanoTime();
    long actual = Solution.run(ids, dictVals, 14);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
