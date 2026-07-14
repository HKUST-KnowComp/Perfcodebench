public class Solution {
    static long checksumBytes(byte[] data) {
        long hash = 1469598103934665603L;
        for (byte b : data) {
            hash ^= (b & 0xFFL);
            hash *= 1099511628211L;
        }
        return hash;
    }

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long finalHash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long hash = 1469598103934665603L;
            for (int i = 0; i < ids.length; i++) {
                int id = ids[i];
                int t = ts[i];
                short lv = levels[i];
                short ln = lens[i];

                // id (4 bytes, little-endian)
                hash ^= (id & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((id >>> 8) & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((id >>> 16) & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((id >>> 24) & 0xFFL);
                hash *= 1099511628211L;

                // ts (4 bytes, little-endian)
                hash ^= (t & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((t >>> 8) & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((t >>> 16) & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((t >>> 24) & 0xFFL);
                hash *= 1099511628211L;

                // levels (2 bytes, little-endian)
                hash ^= (lv & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((lv >>> 8) & 0xFFL);
                hash *= 1099511628211L;

                // lens (2 bytes, little-endian)
                hash ^= (ln & 0xFFL);
                hash *= 1099511628211L;
                hash ^= ((ln >>> 8) & 0xFFL);
                hash *= 1099511628211L;
            }
            finalHash = hash;
        }
        return finalHash;
    }
}
