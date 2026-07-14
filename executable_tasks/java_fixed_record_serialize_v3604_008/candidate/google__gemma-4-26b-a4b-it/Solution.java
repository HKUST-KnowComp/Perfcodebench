import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int recordSize = 4 + 4 + 2 + 2; // int, int, short, short
    byte[] buffer = new byte[n * recordSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        buffer[offset++] = (byte) id;
        buffer[offset++] = (byte) (id >>> 8);
        buffer[offset++] = (byte) (id >>> 16);
        buffer[offset++] = (byte) (id >>> 24);

        buffer[offset++] = (byte) t;
        buffer[offset++] = (byte) (t >>> 8);
        buffer[offset++] = (byte) (t >>> 16);
        buffer[offset++] = (byte) (t >>> 24);

        buffer[offset++] = (byte) lvl;
        buffer[offset++] = (byte) (lvl >>> 8);

        buffer[offset++] = (byte) ln;
        buffer[offset++] = (byte) (ln >>> 8);
      }
      
      // We must use a slice of the buffer if n was 0, but n is assumed > 0 based on context.
      // To match the original behavior exactly, we pass the filled portion.
      // However, the original code clears the list and re-fills, so the buffer size is always n * 12.
      // We create a temporary array to match the exact checksum logic if the buffer is larger than needed,
      // but here buffer.length is exactly the size needed.
      
      // To ensure correctness with the original 'out.size()' logic:
      byte[] bytesToHash = new byte[offset];
      System.arraycopy(buffer, 0, bytesToHash, 0, offset);
      hash = checksumBytes(bytesToHash);
    }
    return hash;
  }
}