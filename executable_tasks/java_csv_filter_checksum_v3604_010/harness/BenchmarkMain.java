import java.util.*;

public class BenchmarkMain {
  static class Result {
    boolean ok;
    long elapsed_ns;
    long expected_checksum;
    long actual_checksum;
  }

  static List<String> makeRows(int n) {
    List<String> rows = new ArrayList<>(n);
    long state = 0x1234567812345678L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      int a = (int) ((state >>> 16) & 0xFFFF);
      int b = (int) ((state >>> 32) & 0x7FF);
      int c = (int) ((state >>> 48) & 0x3FF);
      rows.add("r" + (i % 67) + "," + a + "," + b + "," + c + "," + ((i % 4) == 0 ? "hot" : "cold"));
    }
    return rows;
  }

  static long expectedChecksum(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(",");
      int b = Integer.parseInt(parts[2]);
      int c = Integer.parseInt(parts[3]);
      boolean hot = parts[4].equals("hot");
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static void main(String[] args) {
    List<String> rows = makeRows(327500);
    long expected = expectedChecksum(rows);
    long start = System.nanoTime();
    long actual = Solution.run(rows);
    long elapsed = System.nanoTime() - start;
    Result r = new Result();
    r.ok = actual == expected;
    r.elapsed_ns = elapsed;
    r.expected_checksum = expected;
    r.actual_checksum = actual;
    System.out.println("{\"ok\":" + r.ok + ",\"elapsed_ns\":" + r.elapsed_ns +
        ",\"expected_checksum\":" + r.expected_checksum + ",\"actual_checksum\":" + r.actual_checksum + "}");
    if (!r.ok) {
      throw new RuntimeException("checksum mismatch");
    }
  }
}
