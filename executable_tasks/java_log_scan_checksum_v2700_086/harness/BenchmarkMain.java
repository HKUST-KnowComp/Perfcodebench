import java.util.*;

public class BenchmarkMain {
  static List<String> makeRows(int n) {
    List<String> rows = new ArrayList<>(n);
    long state = 0xCAFEBABE12345678L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      int src = (int) ((state >>> 12) & 1023);
      int lat = (int) ((state >>> 24) & 2047);
      int code = (int) ((state >>> 36) & 511);
      String ok = ((i % 5) == 0) ? "yes" : "no";
      rows.add("src=S" + src + " lat=" + lat + " code=" + code + " ok=" + ok);
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
      int srcPos = row.indexOf("src=S");
      int latPos = row.indexOf(" lat=");
      int codePos = row.indexOf(" code=");
      int okPos = row.indexOf(" ok=");
      int src = parseIntAt(row, srcPos + 5);
      int lat = parseIntAt(row, latPos + 5);
      int code = parseIntAt(row, codePos + 6);
      boolean ok = row.charAt(okPos + 4) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static void main(String[] args) {
    List<String> rows = makeRows(324200);
    long expected = expectedChecksum(rows);
    long start = System.nanoTime();
    long actual = Solution.run(rows);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
