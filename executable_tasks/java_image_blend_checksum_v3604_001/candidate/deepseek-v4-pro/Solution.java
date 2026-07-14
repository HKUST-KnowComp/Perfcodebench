public class Solution {
    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        int pixels = width * height;
        long finalHash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long h = 1469598103934665603L;
            for (int i = 0; i < pixels; i++) {
                int base = i * 4;
                int a = src[base + 3] & 0xFF;
                int invA = 255 - a;

                int s0 = src[base] & 0xFF;
                int d0 = dst[base] & 0xFF;
                int r = (s0 * a + d0 * invA + 127) / 255;
                h ^= (r & 0xFFL);
                h *= 1099511628211L;

                int s1 = src[base + 1] & 0xFF;
                int d1 = dst[base + 1] & 0xFF;
                int g = (s1 * a + d1 * invA + 127) / 255;
                h ^= (g & 0xFFL);
                h *= 1099511628211L;

                int s2 = src[base + 2] & 0xFF;
                int d2 = dst[base + 2] & 0xFF;
                int b = (s2 * a + d2 * invA + 127) / 255;
                h ^= (b & 0xFFL);
                h *= 1099511628211L;

                h ^= 255L;
                h *= 1099511628211L;
            }
            finalHash = h;
        }
        return finalHash;
    }
}