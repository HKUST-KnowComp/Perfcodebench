import java.util.*;

public class Solution {
  // Optimized blend function using integer math to avoid division where possible
  // (s * a + d * (255 - a) + 127) / 255
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int totalBytes = width * height * 4;
    byte[] out = new byte[totalBytes];
    long hash = 0;
    int pixels = width * height;

    // Pre-extract src components to avoid repeated masking in the loop
    int[] srcR = new int[pixels];
    int[] srcG = new int[pixels];
    int[] srcB = new int[pixels];
    int[] srcA = new int[pixels];
    int[] dstR = new int[pixels];
    int[] dstG = new int[pixels];
    int[] dstB = new int[pixels];

    for (int i = 0; i < pixels; i++) {
      int base = i << 2;
      srcR[i] = src[base] & 0xFF;
      srcG[i] = src[base + 1] & 0xFF;
      srcB[i] = src[base + 2] & 0xFF;
      srcA[i] = src[base + 3] & 0xFF;
      dstR[i] = dst[base] & 0xFF;
      dstG[i] = dst[base + 1] & 0xFF;
      dstB[i] = dst[base + 2] & 0xFF;
    }

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = srcA[i];
        int invA = 255 - a;
        
        // Inline blend logic for performance
        out[base]     = (byte) ((srcR[i] * a + dstR[i] * invA + 127) / 255);
        out[base + 1] = (byte) ((srcG[i] * a + dstG[i] * invA + 127) / 255);
        out[base + 2] = (byte) ((srcB[i] * a + dstB[i] * invA + 127) / 255);
        out[base + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}