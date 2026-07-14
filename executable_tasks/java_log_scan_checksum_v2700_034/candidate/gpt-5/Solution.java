import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV64_OFFSET = 1469598103934665603L;
    final long FNV64_PRIME = 1099511628211L;
    long hash = FNV64_OFFSET;

    for (int i = 0, n = rows.size(); i < n; i++) {
      String s = rows.get(i);
      int len = s.length();
      int idx = 0;

      // Expect: "src=S"
      if (len < 6 || s.charAt(0) != 's' || s.charAt(1) != 'r' || s.charAt(2) != 'c' || s.charAt(3) != '=' || s.charAt(4) != 'S') {
        throw new IllegalStateException("Invalid format: src=S prefix");
      }
      idx = 5;

      // Parse src digits
      int src = 0;
      int start = idx;
      while (idx < len) {
        char c = s.charAt(idx);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        idx++;
      }
      if (idx == start) throw new IllegalStateException("Invalid format: src digits");

      // Space
      if (idx >= len || s.charAt(idx) != ' ') throw new IllegalStateException("Invalid format: space after src");
      idx++;

      // Expect: "lat="
      if (idx + 3 >= len || s.charAt(idx) != 'l' || s.charAt(idx + 1) != 'a' || s.charAt(idx + 2) != 't' || s.charAt(idx + 3) != '=') {
        throw new IllegalStateException("Invalid format: lat=");
      }
      idx += 4;

      // Parse lat digits
      int lat = 0;
      start = idx;
      while (idx < len) {
        char c = s.charAt(idx);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        idx++;
      }
      if (idx == start) throw new IllegalStateException("Invalid format: lat digits");

      // Space
      if (idx >= len || s.charAt(idx) != ' ') throw new IllegalStateException("Invalid format: space after lat");
      idx++;

      // Expect: "code="
      if (idx + 4 >= len || s.charAt(idx) != 'c' || s.charAt(idx + 1) != 'o' || s.charAt(idx + 2) != 'd' || s.charAt(idx + 3) != 'e' || s.charAt(idx + 4) != '=') {
        throw new IllegalStateException("Invalid format: code=");
      }
      idx += 5;

      // Parse code digits
      int code = 0;
      start = idx;
      while (idx < len) {
        char c = s.charAt(idx);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        idx++;
      }
      if (idx == start) throw new IllegalStateException("Invalid format: code digits");

      // Space
      if (idx >= len || s.charAt(idx) != ' ') throw new IllegalStateException("Invalid format: space after code");
      idx++;

      // Expect: "ok="
      if (idx + 2 >= len || s.charAt(idx) != 'o' || s.charAt(idx + 1) != 'k' || s.charAt(idx + 2) != '=') {
        throw new IllegalStateException("Invalid format: ok=");
      }
      idx += 3;

      boolean ok;
      // Parse yes/no and ensure end-of-string
      if (idx < len && s.charAt(idx) == 'y') {
        if (idx + 2 != len || s.charAt(idx + 1) != 'e' || s.charAt(idx + 2) != 's') {
          throw new IllegalStateException("Invalid format: ok=yes");
        }
        ok = true;
      } else if (idx < len && s.charAt(idx) == 'n') {
        if (idx + 1 != len - 1 || s.charAt(idx + 1) != 'o' || idx + 2 != len) {
          throw new IllegalStateException("Invalid format: ok=no");
        }
        ok = false;
      } else {
        throw new IllegalStateException("Invalid format: ok value");
      }

      long v = (long)((src & 255)) + (long)lat + (long)code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }

    return hash;
  }
}
