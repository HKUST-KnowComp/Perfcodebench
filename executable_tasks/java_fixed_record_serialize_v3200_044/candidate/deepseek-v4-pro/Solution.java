public class Solution {
    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        if (iters <= 0) return 0L;
        int n = ids.length;
        byte[] out = new byte[n * 12];
        int offset = 0;
        for (int i = 0; i < n; i++) {
            int id = ids[i];
            out[offset]     = (byte) id;
            out[offset + 1] = (byte) (id >>> 8);
            out[offset + 2] = (byte) (id >>> 16);
            out[offset + 3] = (byte) (id >>> 24);
            int t = ts[i];
            out[offset + 4] = (byte) t;
            out[offset + 5] = (byte) (t >>> 8);
            out[offset + 6] = (byte) (t >>> 16);
            out[offset + 7] = (byte) (t >>> 24);
            short level = levels[i];
            out[offset + 8] = (byte) level;
            out[offset + 9] = (byte) (level >>> 8);
            short len = lens[i];
            out[offset + 10] = (byte) len;
            out[offset + 11] = (byte) (len >>> 8);
            offset += 12;
        }
        long hash = 1469598103934665603L;
        for (int i = 0; i < out.length; i++) {
            long b = out[i] & 0xFFL;
            hash ^= b;
            hash *= 1099511628211L;
        }
        return hash;
    }
}
