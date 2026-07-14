public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (iters <= 0) return 0L;

    int pixels = width * height;
    long hash = FNV_OFFSET_BASIS;

    int base = 0;
    for (int i = 0; i < pixels; i++) {
      int a = src[base + 3] & 0xFF;
      int inv = 255 - a;

      int s = src[base] & 0xFF;
      int d = dst[base] & 0xFF;
      int o = (s * a + d * inv + 127) / 255;
      hash ^= (o & 0xFFL);
      hash *= FNV_PRIME;

      s = src[base + 1] & 0xFF;
      d = dst[base + 1] & 0xFF;
      o = (s * a + d * inv + 127) / 255;
      hash ^= (o & 0xFFL);
      hash *= FNV_PRIME;

      s = src[base + 2] & 0xFF;
      d = dst[base + 2] & 0xFF;
      o = (s * a + d * inv + 127) / 255;
      hash ^= (o & 0xFFL);
      hash *= FNV_PRIME;

      // Alpha channel is set to 255 in the output
      hash ^= 255L;
      hash *= FNV_PRIME;

      base += 4;
    }

    return hash;
  }
}
