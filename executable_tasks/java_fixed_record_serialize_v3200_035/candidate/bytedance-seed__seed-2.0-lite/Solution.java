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
    int recordCount = ids.length;
    int totalBytes = recordCount * 12;
    byte[] outBuffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < recordCount; i++) {
        int offset = i * 12;
        // Write all fields in identical LSB-first order to original baseline
        outBuffer[offset] = (byte) ids[i];
        outBuffer[offset + 1] = (byte) (ids[i] >>> 8);
        outBuffer[offset + 2] = (byte) (ids[i] >>> 16);
        outBuffer[offset + 3] = (byte) (ids[i] >>> 24);
        outBuffer[offset + 4] = (byte) ts[i];
        outBuffer[offset + 5] = (byte) (ts[i] >>> 8);
        outBuffer[offset + 6] = (byte) (ts[i] >>> 16);
        outBuffer[offset + 7] = (byte) (ts[i] >>> 24);
        outBuffer[offset + 8] = (byte) levels[i];
        outBuffer[offset + 9] = (byte) (levels[i] >>> 8);
        outBuffer[offset + 10] = (byte) lens[i];
        outBuffer[offset + 11] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(outBuffer);
    }
    return hash;
  }
}