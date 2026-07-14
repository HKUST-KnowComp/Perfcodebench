public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] buf = new byte[n * 12];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        byte b0 = (byte) id;
        byte b1 = (byte) (id >>> 8);
        byte b2 = (byte) (id >>> 16);
        byte b3 = (byte) (id >>> 24);
        buf[idx++] = b0; h ^= (b0 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = b1; h ^= (b1 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = b2; h ^= (b2 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = b3; h ^= (b3 & 0xFFL); h *= 1099511628211L;
        
        int t = ts[i];
        byte t0 = (byte) t;
        byte t1 = (byte) (t >>> 8);
        byte t2 = (byte) (t >>> 16);
        byte t3 = (byte) (t >>> 24);
        buf[idx++] = t0; h ^= (t0 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = t1; h ^= (t1 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = t2; h ^= (t2 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = t3; h ^= (t3 & 0xFFL); h *= 1099511628211L;
        
        short lv = levels[i];
        byte l0 = (byte) lv;
        byte l1 = (byte) (lv >>> 8);
        buf[idx++] = l0; h ^= (l0 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = l1; h ^= (l1 & 0xFFL); h *= 1099511628211L;
        
        short ln = lens[i];
        byte ln0 = (byte) ln;
        byte ln1 = (byte) (ln >>> 8);
        buf[idx++] = ln0; h ^= (ln0 & 0xFFL); h *= 1099511628211L;
        buf[idx++] = ln1; h ^= (ln1 & 0xFFL); h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}