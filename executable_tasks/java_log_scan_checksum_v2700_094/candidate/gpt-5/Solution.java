import java.util.*;

public class Solution {
  private static final long OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = OFFSET_BASIS;
    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 0;

      // Expect prefix: "src=S"
      if (len < 5 || row.charAt(0) != 's' || row.charAt(1) != 'r' || row.charAt(2) != 'c' || row.charAt(3) != '=' || row.charAt(4) != 'S') {
        throw new IllegalArgumentException("Invalid line format");
      }
      i = 5;

      // Parse src digits (one or more)
      int src = 0;
      int nDigits = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          src = src * 10 + (c - '0');
          i++;
          nDigits++;
        } else {
          break;
        }
      }
      if (nDigits == 0) throw new IllegalArgumentException("Invalid src");

      // Expect space then "lat="
      if (i >= len || row.charAt(i) != ' ') throw new IllegalArgumentException("Missing space before lat");
      i++;
      if (i + 3 >= len || row.charAt(i) != 'l' || row.charAt(i + 1) != 'a' || row.charAt(i + 2) != 't' || row.charAt(i + 3) != '=')
        throw new IllegalArgumentException("Invalid lat token");
      i += 4;

      // Parse lat digits (one or more)
      int lat = 0;
      nDigits = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          lat = lat * 10 + (c - '0');
          i++;
          nDigits++;
        } else {
          break;
        }
      }
      if (nDigits == 0) throw new IllegalArgumentException("Invalid lat");

      // Expect space then "code="
      if (i >= len || row.charAt(i) != ' ') throw new IllegalArgumentException("Missing space before code");
      i++;
      if (i + 4 >= len || row.charAt(i) != 'c' || row.charAt(i + 1) != 'o' || row.charAt(i + 2) != 'd' || row.charAt(i + 3) != 'e' || row.charAt(i + 4) != '=')
        throw new IllegalArgumentException("Invalid code token");
      i += 5;

      // Parse code digits (one or more)
      int code = 0;
      nDigits = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          code = code * 10 + (c - '0');
          i++;
          nDigits++;
        } else {
          break;
        }
      }
      if (nDigits == 0) throw new IllegalArgumentException("Invalid code");

      // Expect space then "ok=" then yes|no and end-of-line
      if (i >= len || row.charAt(i) != ' ') throw new IllegalArgumentException("Missing space before ok");
      i++;
      if (i + 2 >= len || row.charAt(i) != 'o' || row.charAt(i + 1) != 'k' || row.charAt(i + 2) != '=')
        throw new IllegalArgumentException("Invalid ok token");
      i += 3;

      boolean ok;
      if (i + 2 == len && row.charAt(i) == 'y' && row.charAt(i + 1) == 'e' && row.charAt(i + 2) == 's') {
        ok = true;
        i += 3;
      } else if (i + 1 == len && row.charAt(i) == 'n' && row.charAt(i + 1) == 'o') {
        ok = false;
        i += 2;
      } else {
        throw new IllegalArgumentException("Invalid ok value");
      }

      if (i != len) throw new IllegalArgumentException("Trailing characters");

      long v = (long) (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}