public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  static void putU32(byte[] dst, int ptr, int v) {
    dst[ptr] = (byte) v;
    dst[ptr + 1] = (byte) (v >>> 8);
    dst[ptr + 2] = (byte) (v >>> 16);
    dst[ptr + 3] = (byte) (v >>> 24);
  }

  static void putU16(byte[] dst, int ptr, int v) {
    dst[ptr] = (byte) v;
    dst[ptr + 1] = (byte) (v >>> 8);
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    byte[] out = new byte[ids.length * 12];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int ptr = 0;
      for (int i = 0; i < ids.length; i++) {
        putU32(out, ptr, ids[i]); ptr += 4;
        putU32(out, ptr, ts[i]); ptr += 4;
        putU16(out, ptr, levels[i] & 0xFFFF); ptr += 2;
        putU16(out, ptr, lens[i] & 0xFFFF); ptr += 2;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}
