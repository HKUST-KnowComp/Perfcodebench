import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long FNV_PRIME = 1099511628211L;
        
        for (String row : rows) {
            int commaCount = 0;
            int fieldStart = 0;
            int b = 0, c = 0;
            boolean isHot = false;
            final int rowLen = row.length();
            
            for (int i = 0; i < rowLen; i++) {
                if (row.charAt(i) == ',') {
                    if (commaCount == 2) {
                        b = parsePositiveInt(row, fieldStart, i);
                    }
                    else if (commaCount == 3) {
                        c = parsePositiveInt(row, fieldStart, i);
                    }
                    
                    fieldStart = i + 1;
                    commaCount++;
                    
                    if (commaCount == 4) {
                        isHot = (i + 4 <= rowLen) 
                            && row.charAt(i+1) == 'h' 
                            && row.charAt(i+2) == 'o' 
                            && row.charAt(i+3) == 't' 
                            && (i + 4 == rowLen || row.charAt(i+4) == ',');
                        break;
                    }
                }
            }
            
            long v = (b > 300 ? (b + c) : 0) + (isHot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }
    
    private static int parsePositiveInt(String s, int start, int end) {
        int num = 0;
        for (int i = start; i < end; i++) {
            num = num * 10 + (s.charAt(i) - '0');
        }
        return num;
    }
}