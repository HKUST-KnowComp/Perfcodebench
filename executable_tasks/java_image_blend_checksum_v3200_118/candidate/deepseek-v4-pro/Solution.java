public class Solution {
    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        if (iters <= 0) return 0L;
        int pixels = width * height;
        long hash = 1469598103934665603L;
        for (int i = 0; i < pixels; i++) {
            int base = i * 4;
            int a = src[base + 3] & 0xFF;
            int sR = src[base] & 0xFF, dR = dst[base] & 0xFF;
            int sG = src[base + 1] & 0xFF, dG = dst[base + 1] & 0xFF;
            int sB = src[base + 2] & 0xFF, dB = dst[base + 2] & 0xFF;
            int bR = (sR * a + dR * (255 - a) + 127) / 255;
            int bG = (sG * a + dG * (255 - a) + 127) / 255;
            int bB = (sB * a + dB * (255 - a) + 127) / 255;
            hash ^= (bR & 0xFFL);
            hash *= 1099511628211L;
            hash ^= (bG & 0xFFL);
            hash *= 1099511628211L;
            hash ^= (bB & 0xFFL);
            hash *= 1099511628211L;
            hash ^= 255L;
            hash *= 1099511628211L;
        }
        return hash;
    }
}