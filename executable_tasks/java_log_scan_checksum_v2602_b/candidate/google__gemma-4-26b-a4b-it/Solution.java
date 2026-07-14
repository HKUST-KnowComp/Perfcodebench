import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // Manual scan to avoid Regex overhead
      int cursor = 0;

      // "src=S"
      if (len < 6 || row.charAt(0) != 's' || row.charAt(1) != 'r' || row.charAt(2) != 'c' || 
          row.charAt(3) != '=' || row.charAt(4) != 'S') continue;
      cursor = 5;

      // Parse src
      int src = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        src = src * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      if (cursor >= len || row.charAt(cursor) != ' ') continue;
      cursor++; // skip space

      // "lat="
      if (cursor + 4 > len || row.charAt(cursor) != 'l' || row.charAt(cursor+1) != 'a' || 
          row.charAt(cursor+2) != 't' || row.charAt(cursor+3) != '=') continue;
      cursor += 4;

      // Parse lat
      int lat = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        lat = lat * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      if (cursor >= len || row.charAt(cursor) != ' ') continue;
      cursor++; // skip space

      // "code="
      if (cursor + 5 > len || row.charAt(cursor) != 'c' || row.charAt(cursor+1) != 'o' || 
          row.charAt(cursor+2) != 'd' || row.charAt(cursor+3) != 'e' || row.charAt(cursor+4) != '=') continue;
      cursor += 5;

      // Parse code
      int code = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        code = code * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      if (cursor >= len || row.charAt(cursor) != ' ') continue;
      cursor++; // skip space

      // "ok="
      if (cursor + 3 > len || row.charAt(cursor) != 'o' || row.charAt(cursor+1) != 'k' || row.charAt(cursor+2) != '=') continue;
      cursor += 3;

      // Parse ok (yes/no)
      boolean ok = false;
      if (cursor + 3 <= len && row.charAt(cursor) == 'y' && row.charAt(cursor+1) == 'e' && row.charAt(cursor+2) == 's') {
        ok = true;
        cursor += 3;
      } else if (cursor + 2 <= len && row.charAt(cursor) == 'n' && row.charAt(cursor+1) == 'o') {
        ok = false;
        cursor += 2;
      } else {
        continue;
      }

      // Ensure end of string
      if (cursor != len) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}