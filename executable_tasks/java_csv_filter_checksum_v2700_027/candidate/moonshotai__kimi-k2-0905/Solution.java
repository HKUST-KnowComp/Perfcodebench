import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    byte[] buf = new byte[64];               // reusable buffer
    for (String row : rows) {
      int len = row.length();
      if (len > buf.length) buf = new byte[len];
      copyCharsToBytes(row, buf, len);

      int field = 0;
      int b = 0, c = 0;
      boolean hot = false;
      int start = 0;
      for (int i = 0; i < len; i++) {
        if (buf[i] == ',') {
          if (field == 2) b = parseInt(buf, start, i);
          else if (field == 3) c = parseInt(buf, start, i);
          field++;
          start = i + 1;
        }
      }
      // last field: hot/cold
      hot = (len - start == 3 &&
             buf[start]     == 'h' &&
             buf[start + 1] == 'o' &&
             buf[start + 2] == 't');

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static void copyCharsToBytes(String s, byte[] dst, int len) {
    for (int i = 0; i < len; i++) {
      dst[i] = (byte) s.charAt(i);
    }
  }

  private static int parseInt(byte[] buf, int off, int end) {
    int val = 0;
    boolean neg = false;
    if (buf[off] == '-') {
      neg = true;
      off++;
    }
    while (off < end) {
      val = val * 10 + (buf[off++] - '0');
    }
    return neg ? -val : val;
  }
}