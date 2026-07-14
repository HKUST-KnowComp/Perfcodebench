import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            int commaCount = 0;
            int pos2Start = -1;
            int pos2End = -1;
            int pos3Start = -1;
            int pos3End = -1;
            int pos4Start = -1;
            int pos4End = -1;

            // Single pass to find comma positions and field boundaries
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    commaCount++;
                    if (commaCount == 2) pos2Start = j + 1;
                    else if (commaCount == 2 && pos2Start != -1) pos2End = j;
                    else if (commaCount == 3) pos3Start = j + 1;
                    else if (commaCount == 3 && pos3Start != -1) pos3End = j;
                    else if (commaCount == 4) pos4Start = j + 1;
                }
            }
            
            // If commas not found as expected, fallback or handle (though task implies valid CSV)
            // Re-calculating indices more robustly for the specific indices 2, 3, 4
            int p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
            int count = 0;
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    count++;
                    if (count == 1) p1 = j + 1;
                    else if (count == 2) p2 = j + 1;
                    else if (count == 3) p3 = j + 1;
                    else if (count == 4) p4 = j + 1;
                }
            }
            // The indices needed are parts[2], parts[3], parts[4]
            // parts[0] is index 0, parts[1] is index 1, parts[2] is index 2...
            // So we need the 3rd field, 4th field, and 5th field.
            // Let's use a more direct approach to avoid the split() overhead.
            
            int fieldIdx = 0;
            int start = 0;
            int b = 0, c = 0;
            boolean hot = false;
            
            for (int j = 0; j <= len; j++) {
                if (j == len || row.charAt(j) == ',') {
                    if (fieldIdx == 2) {
                        b = parseFastInt(row, start, j);
                    } else if (fieldIdx == 3) {
                        c = parseFastInt(row, start, j);
                    } else if (fieldIdx == 4) {
                        hot = isHot(row, start, j);
                    }
                    start = j + 1;
                    fieldIdx++;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseFastInt(String s, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }

    private static boolean isHot(String s, int start, int end) {
        int len = end - start;
        if (len != 3) return false;
        return s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't';
    }
}