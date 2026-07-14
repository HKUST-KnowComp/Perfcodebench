import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET_BASIS;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String s = rows.get(r);
      int n = s.length();
      int i = 0;

      // Expect: src=S
      if (n < 10) throw new IllegalArgumentException("line too short");
      if (i + 5 > n || s.charAt(i++) != 's' || s.charAt(i++) != 'r' || s.charAt(i++) != 'c' || s.charAt(i++) != '=' || s.charAt(i++) != 'S') {
        throw new IllegalArgumentException("bad prefix src=S");
      }

      // Parse src digits until space
      int src = 0;
      int start = i;
      while (i < n) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          src = src * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
      if (i == start || i >= n || s.charAt(i++) != ' ') {
        throw new IllegalArgumentException("bad src field");
      }

      // Expect: lat=
      if (i + 4 > n || s.charAt(i++) != 'l' || s.charAt(i++) != 'a' || s.charAt(i++) != 't' || s.charAt(i++) != '=') {
        throw new IllegalArgumentException("bad lat= prefix");
      }

      // Parse lat digits until space
      int lat = 0;
      start = i;
      while (i < n) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          lat = lat * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
      if (i == start || i >= n || s.charAt(i++) != ' ') {
        throw new IllegalArgumentException("bad lat field");
      }

      // Expect: code=
      if (i + 5 > n || s.charAt(i++) != 'c' || s.charAt(i++) != 'o' || s.charAt(i++) != 'd' || s.charAt(i++) != 'e' || s.charAt(i++) != '=') {
        throw new IllegalArgumentException("bad code= prefix");
      }

      // Parse code digits until space
      int code = 0;
      start = i;
      while (i < n) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          code = code * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
      if (i == start || i >= n || s.charAt(i++) != ' ') {
        throw new IllegalArgumentException("bad code field");
      }

      // Expect: ok=
      if (i + 3 > n || s.charAt(i++) != 'o' || s.charAt(i++) != 'k' || s.charAt(i++) != '=') {
        throw new IllegalArgumentException("bad ok= prefix");
      }

      boolean ok;
      int remaining = n - i;
      if (remaining == 3 && s.charAt(i) == 'y' && s.charAt(i + 1) == 'e' && s.charAt(i + 2) == 's') {
        ok = true;
        i += 3;
      } else if (remaining == 2 && s.charAt(i) == 'n' && s.charAt(i + 1) == 'o') {
        ok = false;
        i += 2;
      } else {
        throw new IllegalArgumentException("bad ok value");
      }

      if (i != n) {
        throw new IllegalArgumentException("trailing characters");
      }

      long v = (long) (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
