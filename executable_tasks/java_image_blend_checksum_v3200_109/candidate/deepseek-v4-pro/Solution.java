public class Solution {
    private static final long MULTIPLIER = 1099511628211L;
    private static final long INITIAL_HASH = 1469598103934665603L;

    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        int pixels = width * height;
        long hashResult = 0L;
        for (int iter = 0; iter < iters; iter++) {
            long hash = INITIAL_HASH;
            int base = 0;
            for (int i = 0; i < pixels; i++) {
                int sR = src[base] & 0xFF;
                int sG = src[base + 1] & 0xFF;
                int sB = src[base + 2] & 0xFF;
                int a = src[base + 3] & 0xFF;
                int invA = 255 - a;
                int dR = dst[base] & 0xFF;
                int dG = dst[base + 1] & 0xFF;
                int dB = dst[base + 2] & 0xFF;
                int r = (sR * a + dR * invA + 127) / 255;
                int g = (sG * a + dG * invA + 127) / 255;
                int b = (sB * a + dB * invA + 127) / 255;
                hash ^= (r & 0xFFL);
                hash *= MULTIPLIER;
                hash ^= (g & 0xFFL);
                hash *= MULTIPLIER;
                hash ^= (b & 0xFFL);
                hash *= MULTIPLIER;
                hash ^= 255L;
                hash *= MULTIPLIER;
                base += 4;
            }
            hashResult = hash;
        }
        return hashResult;
    }
}