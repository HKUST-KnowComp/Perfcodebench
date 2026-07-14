public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int size = n * 12;
    byte[] buf = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      long h = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        byte b;
        b = (byte) id; h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (id >>> 8); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (id >>> 16); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (id >>> 24); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        int t = ts[i];
        b = (byte) t; h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (t >>> 8); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (t >>> 16); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (t >>> 24); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        short lv = levels[i];
        b = (byte) lv; h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (lv >>> 8); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        short ln = lens[i];
        b = (byte) ln; h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
        b = (byte) (ln >>> 8); h ^= (b & 0xFF); h *= 1099511628211L; buf[idx++] = b;
      }
      hash = h;
    }
    return hash;
  }
}