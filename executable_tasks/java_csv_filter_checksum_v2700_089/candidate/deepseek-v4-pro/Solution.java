public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long multiplier = 1099511628211L;
        for (String row : rows) {
            int idx1 = row.indexOf(',');
            int idx2 = row.indexOf(',', idx1 + 1);
            int idx3 = row.indexOf(',', idx2 + 1);
            int idx4 = row.indexOf(',', idx3 + 1);
            int b = 0;
            for (int i = idx1 + 1; i < idx2; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }
            int c = 0;
            for (int i = idx2 + 1; i < idx3; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }
            int hotStart = idx3 + 1;
            int hotEnd = (idx4 == -1) ? row.length() : idx4;
            boolean hot = (hotEnd - hotStart == 3)
                && row.charAt(hotStart) == 'h'
                && row.charAt(hotStart + 1) == 'o'
                && row.charAt(hotStart + 2) == 't';
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= multiplier;
        }
        return hash;
    }
}