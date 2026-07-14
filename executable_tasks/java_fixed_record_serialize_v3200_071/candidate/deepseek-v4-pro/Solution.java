import java.util.*;

public class Solution {
    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        final int n = ids.length;
        long hash = 0;
        final long init = 1469598103934665603L;
        final long mult = 1099511628211L;
        for (int iter = 0; iter < iters; iter++) {
            long h = init;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                h ^= id & 0xFFL;          h *= mult;
                h ^= (id >>> 8) & 0xFFL;  h *= mult;
                h ^= (id >>> 16) & 0xFFL; h *= mult;
                h ^= (id >>> 24) & 0xFFL; h *= mult;

                int t = ts[i];
                h ^= t & 0xFFL;          h *= mult;
                h ^= (t >>> 8) & 0xFFL;  h *= mult;
                h ^= (t >>> 16) & 0xFFL; h *= mult;
                h ^= (t >>> 24) & 0xFFL; h *= mult;

                short lvl = levels[i];
                h ^= lvl & 0xFFL;      h *= mult;
                h ^= (lvl >>> 8) & 0xFFL; h *= mult;

                short len = lens[i];
                h ^= len & 0xFFL;      h *= mult;
                h ^= (len >>> 8) & 0xFFL; h *= mult;
            }
            hash = h;
        }
        return hash;
    }
}
