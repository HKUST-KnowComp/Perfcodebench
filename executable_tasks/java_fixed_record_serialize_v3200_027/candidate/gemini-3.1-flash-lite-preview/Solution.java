public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] buffer = new byte[n * 12];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      long currentHash = 1469598103934665603L;
      
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        int lvl = levels[i];
        int ln = lens[i];

        for (int j = 0; j < 4; j++) {
          byte b = (byte) (id >>> (j * 8));
          buffer[pos++] = b;
          currentHash ^= (b & 0xFFL);
          currentHash *= 1099511628211L;
        }
        for (int j = 0; j < 4; j++) {
          byte b = (byte) (t >>> (j * 8));
          buffer[pos++] = b;
          currentHash ^= (b & 0xFFL);
          currentHash *= 1099511628211L;
        }
        byte b1 = (byte) lvl;
        buffer[pos++] = b1;
        currentHash ^= (b1 & 0xFFL);
        currentHash *= 1099511628211L;
        byte b2 = (byte) (lvl >>> 8);
        buffer[pos++] = b2;
        currentHash ^= (b2 & 0xFFL);
        currentHash *= 1099511628211L;
        byte b3 = (byte) ln;
        buffer[pos++] = b3;
        currentHash ^= (b3 & 0xFFL);
        currentHash *= 1099511628211L;
        byte b4 = (byte) (ln >>> 8);
        buffer[pos++] = b4;
        currentHash ^= (b4 & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }
    return hash;
  }
}