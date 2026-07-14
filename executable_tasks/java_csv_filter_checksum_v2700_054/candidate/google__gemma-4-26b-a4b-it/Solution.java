import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int pos = 0;
            
            int b = 0;
            int c = 0;
            boolean hot = false;

            // Manual scan to find indices of commas and parse values
            // We need parts[2], parts[3], and parts[4]
            // parts[0], parts[1], parts[2], parts[3], parts[4]
            
            int currentCommaIdx = -1;
            int secondCommaIdx = -1;
            int thirdCommaIdx = -1;
            int fourthCommaIdx = -1;
            int fifthCommaIdx = -1;

            int commaNum = 0;
            for (int i = 0; i < len; i++) {
                if (row.charAt(i) == ',') {
                    commaNum++;
                    if (commaNum == 1) secondCommaIdx = i;
                    else if (commaNum == 2) thirdCommaIdx = i;
                    else if (commaNum == 3) fourthCommaIdx = i;
                    else if (commaNum == 4) fifthCommaIdx = i;
                }
            }

            // Parse parts[2] (between 2nd and 3rd comma)
            int start = -1;
            int end = -1;
            
            // Find start of index 2
            int count = 0;
            for(int i=0; i<len; i++) {
                if(row.charAt(i) == ',') {
                    count++;
                    if(count == 2) { start = i + 1; break; }
                }
            }
            // Find end of index 2
            for(int i=start; i<len; i++) {
                if(row.charAt(i) == ',') { end = i; break; }
            }
            b = fastParseInt(row, start, end);

            // Find start of index 3
            count = 0;
            for(int i=0; i<len; i++) {
                if(row.charAt(i) == ',') {
                    count++;
                    if(count == 3) { start = i + 1; break; }
                }
            }
            // Find end of index 3
            for(int i=start; i<len; i++) {
                if(row.charAt(i) == ',') { end = i; break; }
            }
            c = fastParseInt(row, start, end);

            // Find start of index 4
            count = 0;
            for(int i=0; i<len; i++) {
                if(row.charAt(i) == ',') {
                    count++;
                    if(count == 4) { start = i + 1; break; }
                }
            }
            // Find end of index 4 (either next comma or end of string)
            end = len;
            for(int i=start; i<len; i++) {
                if(row.charAt(i) == ',') { end = i; break; }
            }
            
            // Check for "hot"
            hot = isHot(row, start, end);

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int fastParseInt(String s, int start, int end) {
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
