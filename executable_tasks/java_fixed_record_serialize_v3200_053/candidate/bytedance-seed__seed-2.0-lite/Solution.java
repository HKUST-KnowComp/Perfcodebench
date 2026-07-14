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
    final int recordCount = ids.length;
    final int totalBytes = recordCount * 12;
    final byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < recordCount; i++) {
        final int offset = i * 12;
        // Write all fields in exact original byte order to preserve correctness
        buffer[offset] = (byte) ids[i];
        buffer[offset + 1] = (byte) (ids[i] >>> 8);
        buffer[offset + 2] = (byte) (ids[i] >>> 16);
        buffer[offset + 3] = (byte) (ids[i] >>> 24);
        buffer[offset + 4] = (byte) ts[i];
        buffer[offset + 5] = (byte) (ts[i] >>> 8);
        buffer[offset + 6] = (byte) (ts[i] >>> 16);
        buffer[offset + 7] = (byte) (ts[i] >>> 24);
        buffer[offset + 8] = (byte) levels[i];
        buffer[offset + 9] = (byte) (levels[i] >>> 8);
        buffer[offset + 10] = (byte) lens[i];
        buffer[offset + 11] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}