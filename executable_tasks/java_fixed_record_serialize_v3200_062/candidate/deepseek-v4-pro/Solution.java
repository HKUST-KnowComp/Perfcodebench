import java.util.*;

public class Solution {
    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long hash = 1469598103934665603L;
        for (int i = 0; i < ids.length; i++) {
            int id = ids[i];
            hash ^= (id & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((id >>> 8) & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((id >>> 16) & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((id >>> 24) & 0xFFL);
            hash *= 1099511628211L;

            int t = ts[i];
            hash ^= (t & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((t >>> 8) & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((t >>> 16) & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((t >>> 24) & 0xFFL);
            hash *= 1099511628211L;

            short lv = levels[i];
            hash ^= (lv & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((lv >>> 8) & 0xFFL);
            hash *= 1099511628211L;

            short ln = lens[i];
            hash ^= (ln & 0xFFL);
            hash *= 1099511628211L;
            hash ^= ((ln >>> 8) & 0xFFL);
            hash *= 1099511628211L;
        }
        return hash;
    }
}