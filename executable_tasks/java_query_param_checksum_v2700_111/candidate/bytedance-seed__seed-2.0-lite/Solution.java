import java.util.List;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = FNV_OFFSET;
        for (String row : rows) {
            int firstAmp = row.indexOf('&');
            int secondAmp = row.indexOf('&', firstAmp + 1);
            
            int u = Integer.parseInt(row.substring(2, firstAmp));
            int s = Integer.parseInt(row.substring(firstAmp + 7, secondAmp));
            int f = Integer.parseInt(row.substring(secondAmp + 6));
            
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }
}