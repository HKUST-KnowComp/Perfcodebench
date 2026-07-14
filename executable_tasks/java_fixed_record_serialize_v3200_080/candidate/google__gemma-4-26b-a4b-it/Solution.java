import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
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
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        // Serialize ID (int)
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);

        // Serialize TS (int)
        buffer[pos++] = (byte) t;
        buffer[pos++] = (byte) (t >>> 8);
        buffer[pos++] = (byte) (t >>> 16);
        buffer[pos++] = (byte) (t >>> 24);

        // Serialize Level (short)
        buffer[pos++] = (byte) lvl;
        buffer[pos++] = (byte) (lvl >>> 8);

        // Serialize Lens (short)
        buffer[pos++] = (byte) ln;
        buffer[pos++] = (byte) (ln >>> 8);
      }
      
      // We must pass the exact slice used to checksumBytes to match original behavior
      // Since we use the full buffer, we create a view or use the array directly if pos == length
      // To be safe and match the exact byte array size of the original 'out' list:
      byte[] resultBytes = new byte[pos];
      System.arraycopy(buffer, 0, resultBytes, 0, pos);
      hash = checksumBytes(resultBytes);
    }
    return hash;
  }
}