import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET_BASIS;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 0;

      // Expect {"u":
      if (len < 5) throw new IllegalArgumentException("row too short");
      if (row.charAt(i++) != '{' || row.charAt(i++) != '"' || row.charAt(i++) != 'u' || row.charAt(i++) != '"' || row.charAt(i++) != ':') {
        throw new IllegalArgumentException("bad prefix");
      }

      // parse u
      int u = 0;
      if (i >= len) throw new IllegalArgumentException("unexpected end after u:");
      {
        int start = i;
        while (i < len) {
          char c = row.charAt(i);
          if (c < '0' || c > '9') break;
          u = u * 10 + (c - '0');
          i++;
        }
        if (i == start) throw new IllegalArgumentException("no digits for u");
      }

      // ,"s":
      if (i + 4 >= len || row.charAt(i++) != ',' || row.charAt(i++) != '"' || row.charAt(i++) != 's' || row.charAt(i++) != '"' || row.charAt(i++) != ':') {
        throw new IllegalArgumentException("bad s field header");
      }

      // parse s
      int s = 0;
      if (i >= len) throw new IllegalArgumentException("unexpected end after s:");
      {
        int start = i;
        while (i < len) {
          char c = row.charAt(i);
          if (c < '0' || c > '9') break;
          s = s * 10 + (c - '0');
          i++;
        }
        if (i == start) throw new IllegalArgumentException("no digits for s");
      }

      // ,"f":
      if (i + 4 >= len || row.charAt(i++) != ',' || row.charAt(i++) != '"' || row.charAt(i++) != 'f' || row.charAt(i++) != '"' || row.charAt(i++) != ':') {
        throw new IllegalArgumentException("bad f field header");
      }

      // parse boolean true|false
      boolean f;
      if (i >= len) throw new IllegalArgumentException("unexpected end at f value");
      char c = row.charAt(i);
      if (c == 't') {
        if (i + 3 >= len || row.charAt(i + 1) != 'r' || row.charAt(i + 2) != 'u' || row.charAt(i + 3) != 'e') {
          throw new IllegalArgumentException("bad true token");
        }
        f = true;
        i += 4;
      } else if (c == 'f') {
        if (i + 4 >= len || row.charAt(i + 1) != 'a' || row.charAt(i + 2) != 'l' || row.charAt(i + 3) != 's' || row.charAt(i + 4) != 'e') {
          throw new IllegalArgumentException("bad false token");
        }
        f = false;
        i += 5;
      } else {
        throw new IllegalArgumentException("expected true/false");
      }

      // ,"tag":"x
      if (i + 8 >= len || row.charAt(i++) != ',' || row.charAt(i++) != '"' || row.charAt(i++) != 't' || row.charAt(i++) != 'a' || row.charAt(i++) != 'g' || row.charAt(i++) != '"' || row.charAt(i++) != ':' || row.charAt(i++) != '"' || row.charAt(i++) != 'x') {
        throw new IllegalArgumentException("bad tag field header");
      }

      // skip digits of tag
      while (i < len) {
        char tc = row.charAt(i);
        if (tc < '0' || tc > '9') break;
        i++;
      }

      // closing "}
      if (i + 1 != len - 0) { // we expect exactly two remaining chars: '"' and '}'
        if (i + 2 != len || row.charAt(i) != '"' || row.charAt(i + 1) != '}') {
          throw new IllegalArgumentException("bad closing");
        }
      } else {
        throw new IllegalArgumentException("unexpected end before closing");
      }
      // move i to end for clarity (unused further)
      i = len;

      long v = (long) (u & 1023) + (long) s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}
