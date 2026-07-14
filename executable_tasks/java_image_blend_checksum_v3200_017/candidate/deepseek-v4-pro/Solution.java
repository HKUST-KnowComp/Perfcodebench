public class Solution {
    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        if (iters == 0) return 0L;
        int pixels = width * height;
        long hash = 1469598103934665603L;
        int limit = pixels << 2; // *4
        for (int i = 0; i < limit; i += 4) {
            int a = src[i + 3] & 0xFF;
            int invA = 255 - a;

            int r = (((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
            hash ^= (r & 0xFFL);
            hash *= 1099511628211L;

            int g = (((src[i + 1] & 0xFF) * a + (dst[i + 1] & 0xFF) * invA + 127) / 255);
            hash ^= (g & 0xFFL);
            hash *= 1099511628211L;

            int b = (((src[i + 2] & 0xFF) * a + (dst[i + 2] & 0xFF) * invA + 127) / 255);
            hash ^= (b & 0xFFL);
            hash *= 1099511628211L;

            // alpha is always 255
            hash ^= 255L;
            hash *= 1099511628211L;
        }
        return hash;
    }
}