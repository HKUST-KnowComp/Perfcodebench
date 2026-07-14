import java.util.*;

public class BenchmarkMain {
  static List<String> makeRows(int n) {
    List<String> rows = new ArrayList<>(n);
    long state = 0x88779966AABBCCDDL;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      int u = (int) ((state >>> 16) & 65535);
      int s = (int) ((state >>> 28) & 2047);
      int f = (i % 15) == 0 ? 1 : 0;
      rows.add("u=" + u + "&score=" + s + "&flag=" + f + "&k=z" + (i % 113));
    }
    return rows;
  }

  static int parseIntAt(String s, int start) {
    int v = 0;
    for (int i = start; i < s.length(); i++) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') break;
      v = v * 10 + (c - '0');
    }
    return v;
  }

  static long expectedChecksum(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int up = row.indexOf("u=") + 2;
      int sp = row.indexOf("&score=") + 7;
      int fp = row.indexOf("&flag=") + 6;
      int u = parseIntAt(row, up);
      int s = parseIntAt(row, sp);
      int f = parseIntAt(row, fp);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static void main(String[] args) {
    List<String> rows = makeRows(217100);
    long expected = expectedChecksum(rows);
    long start = System.nanoTime();
    long actual = Solution.run(rows);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
