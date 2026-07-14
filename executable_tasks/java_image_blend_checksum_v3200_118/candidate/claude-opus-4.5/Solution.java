import java.util.*;

public class Solution {

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;

    // Pre-compute blend table: blend[s][a] = (s * a + 127) / 255
    // We'll compute: blended = blend[s][a] + blend[255-d][a] approximation won't work directly
    // Actually: result = (s*a + d*(255-a) + 127)/255
    // Let's precompute two tables: mulTable[v][a] = v * a  (needs 256*256 = 64K entries)
    // Then result = (mulTable[s][a] + mulTable[d][255-a] + 127) / 255
    // But division is slow. Let's precompute full blend: blendTable[s_minus_d + 255][a] * factor + d
    // Simpler: precompute blend[a][s] = s * a, blend[a][d] = d * (255-a), sum + 127, /255
    // Best: precompute result directly for all (s,d,a) is 16M entries - too large
    // Use: result = d + ((s - d) * a + 127) / 255  when s >= d
    //      result = d - ((d - s) * a + 127) / 255  when s < d
    // Precompute: divTable[x] = (x + 127) / 255 for x in 0..255*255
    
    // Actually simplest optimization: use int array, inline everything, avoid ArrayList
    
    byte[] out = new byte[len];
    long hash = 0;
    
    // Precompute: for blend, result = (s*a + d*(255-a) + 127) / 255
    // = (s*a + d*255 - d*a + 127) / 255
    // = d + (a*(s-d) + 127) / 255   [when s >= d, need signed handling]
    // Use lookup: mult[a][delta+255] where delta = s - d in [-255, 255]
    // mult[a][delta+255] = (a * delta + 127) / 255  with rounding toward zero for negative
    // Actually (a * delta + 127) / 255 doesn't work for negative delta
    // For negative: (a * delta - 127) / 255 ? No, let's just compute directly.
    
    // Simplest fast approach: inline and use primitives
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = FNV_OFFSET;
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int s0 = src[base] & 0xFF;
        int d0 = dst[base] & 0xFF;
        int r0 = (s0 * a + d0 * invA + 127) / 255;
        
        int s1 = src[base + 1] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        int r1 = (s1 * a + d1 * invA + 127) / 255;
        
        int s2 = src[base + 2] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        int r2 = (s2 * a + d2 * invA + 127) / 255;
        
        // Inline checksum for these 4 bytes
        hash ^= r0;
        hash *= FNV_PRIME;
        hash ^= r1;
        hash *= FNV_PRIME;
        hash ^= r2;
        hash *= FNV_PRIME;
        hash ^= 255;
        hash *= FNV_PRIME;
      }
    }
    return hash;
  }
}