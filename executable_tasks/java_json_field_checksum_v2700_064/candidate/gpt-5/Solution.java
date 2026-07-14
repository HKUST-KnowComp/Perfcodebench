import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L; // FNV-1a 64-bit offset basis
    for (String row : rows) {
      int i = 5; // skip initial '{"u":'

      // parse u until ','
      int u = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch == ',') { i++; break; }
        u = u * 10 + (ch - '0');
        i++;
      }

      // skip '"s":' (4 characters)
      i += 4;

      // parse s until ','
      int s = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch == ',') { i++; break; }
        s = s * 10 + (ch - '0');
        i++;
      }

      // skip '"f":' (4 characters)
      i += 4;

      // parse boolean (true/false) by first char
      boolean f = row.charAt(i) == 't';
      // No need to advance i further for performance since remaining content is unused

      long v = (u & 1023) + (long) s + (f ? 17 : 0);
      hash ^= (v + 1L);
      hash *= 1099511628211L; // FNV-1a 64-bit prime
    }
    return hash;
  }
}