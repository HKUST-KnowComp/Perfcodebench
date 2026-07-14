import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 0;

      // Expect "src=S"
      if (len < 5 || s.charAt(0) != 's' || s.charAt(1) != 'r' || s.charAt(2) != 'c' || s.charAt(3) != '=' || s.charAt(4) != 'S') {
        throw new IllegalArgumentException("Bad prefix");
      }
      i = 5;

      // Parse src digits
      int src = 0;
      int start = i;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        src = src * 10 + (ch - '0');
        i++;
      }
      if (i == start) throw new IllegalArgumentException("Missing src digits");
      if (i >= len || s.charAt(i) != ' ') throw new IllegalArgumentException("Expected space after src");
      i++;

      // Expect "lat="
      if (i + 4 > len || s.charAt(i) != 'l' || s.charAt(i + 1) != 'a' || s.charAt(i + 2) != 't' || s.charAt(i + 3) != '=') {
        throw new IllegalArgumentException("Bad lat token");
      }
      i += 4;

      // Parse lat digits
      int lat = 0;
      start = i;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        lat = lat * 10 + (ch - '0');
        i++;
      }
      if (i == start) throw new IllegalArgumentException("Missing lat digits");
      if (i >= len || s.charAt(i) != ' ') throw new IllegalArgumentException("Expected space after lat");
      i++;

      // Expect "code="
      if (i + 5 > len || s.charAt(i) != 'c' || s.charAt(i + 1) != 'o' || s.charAt(i + 2) != 'd' || s.charAt(i + 3) != 'e' || s.charAt(i + 4) != '=') {
        throw new IllegalArgumentException("Bad code token");
      }
      i += 5;

      // Parse code digits
      int code = 0;
      start = i;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        code = code * 10 + (ch - '0');
        i++;
      }
      if (i == start) throw new IllegalArgumentException("Missing code digits");
      if (i >= len || s.charAt(i) != ' ') throw new IllegalArgumentException("Expected space after code");
      i++;

      // Expect "ok="
      if (i + 3 > len || s.charAt(i) != 'o' || s.charAt(i + 1) != 'k' || s.charAt(i + 2) != '=') {
        throw new IllegalArgumentException("Bad ok token");
      }
      i += 3;

      boolean ok;
      // yes or no must terminate the string
      if (i + 3 == len && s.charAt(i) == 'y' && s.charAt(i + 1) == 'e' && s.charAt(i + 2) == 's') {
        ok = true;
        i += 3;
      } else if (i + 2 == len && s.charAt(i) == 'n' && s.charAt(i + 1) == 'o') {
        ok = false;
        i += 2;
      } else {
        throw new IllegalArgumentException("Bad ok value");
      }

      if (i != len) throw new IllegalArgumentException("Trailing characters");

      long v = (long) (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}