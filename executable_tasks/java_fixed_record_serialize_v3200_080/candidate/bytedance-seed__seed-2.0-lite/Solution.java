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
    if (ids.length == 0) return 0;
    final int RECORD_SIZE = 12;
    byte[] buffer = new byte[ids.length * RECORD_SIZE];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int ptr = 0;
      for (int i = 0; i < ids.length; i++) {
        // Write 4-byte id, matching original byte order
        int id = ids[i];
        buffer[ptr++] = (byte) id;
        buffer[ptr++] = (byte) (id >>> 8);
        buffer[ptr++] = (byte) (id >>> 16);
        buffer[ptr++] = (byte) (id >>> 24);
        
        // Write 4-byte timestamp
        int t = ts[i];
        buffer[ptr++] = (byte) t;
        buffer[ptr++] = (byte) (t >>> 8);
        buffer[ptr++] = (byte) (t >>> 16);
        buffer[ptr++] = (byte) (t >>> 24);
        
        // Write 2-byte level
        short lvl = levels[i];
        buffer[ptr++] = (byte) lvl;
        buffer[ptr++] = (byte) (lvl >>> 8);
        
        // Write 2-byte length
        short len = lens[i];
        buffer[ptr++] = (byte) len;
        buffer[ptr++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}