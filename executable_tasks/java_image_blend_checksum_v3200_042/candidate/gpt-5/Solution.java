public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;

    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    int len = width * height * 4;
    for (int i = 0; i < len; i += 4) {
      int a = src[i + 3] & 0xFF;

      int sr = src[i] & 0xFF;
      int sg = src[i + 1] & 0xFF;
      int sb = src[i + 2] & 0xFF;

      int dr = dst[i] & 0xFF;
      int dg = dst[i + 1] & 0xFF;
      int db = dst[i + 2] & 0xFF;

      int r, g, b;
      if (a == 0) {
        r = dr; g = dg; b = db;
      } else if (a == 255) {
        r = sr; g = sg; b = sb;
      } else {
        r = dr + ((sr - dr) * a + 127) / 255;
        g = dg + ((sg - dg) * a + 127) / 255;
        b = db + ((sb - db) * a + 127) / 255;
      }

      hash ^= (r & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= (g & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= (b & 0xFFL);
      hash *= FNV_PRIME;
      hash ^= 255L; // alpha is forced to 255 in the baseline
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
