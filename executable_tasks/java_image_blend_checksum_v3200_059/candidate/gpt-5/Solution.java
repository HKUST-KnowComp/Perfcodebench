public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    final int pixels = width * height;
    final int end = pixels << 2; // total bytes
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int off = 0; off < end; off += 4) {
        int a = src[off + 3] & 0xFF;
        int invA = 255 - a;

        // R channel
        int s = src[off] & 0xFF;
        int d = dst[off] & 0xFF;
        int blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV_PRIME;

        // G channel
        s = src[off + 1] & 0xFF;
        d = dst[off + 1] & 0xFF;
        blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV_PRIME;

        // B channel
        s = src[off + 2] & 0xFF;
        d = dst[off + 2] & 0xFF;
        blended = (s * a + d * invA + 127) / 255;
        h ^= (long) (blended & 0xFF);
        h *= FNV_PRIME;

        // A channel (always 255)
        h ^= 255L;
        h *= FNV_PRIME;
      }
      hash = h; // checksum of the last iteration's output
    }

    return hash;
  }
}
