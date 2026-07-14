import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
    // Re-aligning to exact baseline constant: 1469598103934665603L
    hash = 1469598103934665603L;

    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Manual parsing to avoid Regex overhead
      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      
      int cursor = 4; // skip "src=" (length 4) and 'S' is at index 4? No, "src=S" is 5 chars.
      // Let's re-examine: "src=S" -> s=0, r=1, c=2, ==3, S=4. 
      // The regex is ^src=S(\d+)
      
      if (len < 15 || row.charAt(0) != 's' || row.charAt(1) != 'r' || row.charAt(2) != 'c' || row.charAt(3) != '=') continue;
      if (row.charAt(4) != 'S') continue;
      
      cursor = 5;
      int src = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        src = src * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      
      if (cursor >= len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'l' || row.charAt(cursor+2) != 'a' || row.charAt(cursor+3) != 't' || row.charAt(cursor+4) != '=') continue;
      cursor += 5;
      
      int lat = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        lat = lat * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      
      if (cursor >= len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'c' || row.charAt(cursor+2) != 'o' || row.charAt(cursor+3) != 'd' || row.charAt(cursor+4) != 'e' || row.charAt(cursor+5) != '=') continue;
      cursor += 6;
      
      int code = 0;
      while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
        code = code * 10 + (row.charAt(cursor) - '0');
        cursor++;
      }
      
      if (cursor >= len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'o' || row.charAt(cursor+2) != 'k' || row.charAt(cursor+3) != '=') continue;
      cursor += 4;
      
      boolean ok = false;
      if (cursor + 2 < len && row.charAt(cursor) == 'y' && row.charAt(cursor+1) == 'e' && row.charAt(cursor+2) == 's') {
        ok = true;
        cursor += 3;
      } else if (cursor + 1 < len && row.charAt(cursor) == 'n' && row.charAt(cursor+1) == 'o') {
        ok = false;
        cursor += 2;
      } else {
        continue;
      }
      
      if (cursor != len) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
