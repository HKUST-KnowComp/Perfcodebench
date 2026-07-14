import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    byte[] out = new byte[src.length];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      long currentHash = FNV_OFFSET;
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        int aInv = 255 - a;
        final int roundOffset = 127;

        // Blend R channel
        int sR = src[base] & 0xFF;
        int dR = dst[base] & 0xFF;
        int blendR = (sR * a + dR * aInv + roundOffset) * 0x8081 >> 23;
        out[base] = (byte) blendR;
        currentHash ^= (out[base] & 0xFFL);
        currentHash *= FNV_PRIME;

        // Blend G channel
        int sG = src[base + 1] & 0xFF;
        int dG = dst[base + 1] & 0xFF;
        int blendG = (sG * a + dG * aInv + roundOffset) * 0x8081 >> 23;
        out[base + 1] = (byte) blendG;
        currentHash ^= (out[base + 1] & 0xFFL);
        currentHash *= FNV_PRIME;

        // Blend B channel
        int sB = src[base + 2] & 0xFF;
        int dB = dst[base + 2] & 0xFF;
        int blendB = (sB * a + dB * aInv + roundOffset) * 0x8081 >> 23;
        out[base + 2] = (byte) blendB;
        currentHash ^= (out[base + 2] & 0xFFL);
        currentHash *= FNV_PRIME;

        // Set full alpha, update hash
        out[base + 3] = (byte) 255;
        currentHash ^= 0xFFL;
        currentHash *= FNV_PRIME;
      }
      hash = currentHash;
    }
    return hash;
  }
}