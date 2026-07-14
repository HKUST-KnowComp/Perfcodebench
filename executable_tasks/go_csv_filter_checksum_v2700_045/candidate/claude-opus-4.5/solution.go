package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4
		// Format: field0,field1,field2,field3,field4,...
		// We need fields at index 2, 3, 4
		
		// Skip field 0
		i := 0
		for i < len(row) && row[i] != ',' {
			i++
		}
		i++ // skip comma
		
		// Skip field 1
		for i < len(row) && row[i] != ',' {
			i++
		}
		i++ // skip comma
		
		// Parse field 2 (b)
		start := i
		for i < len(row) && row[i] != ',' {
			i++
		}
		b := parseIntFast(row[start:i])
		i++ // skip comma
		
		// Parse field 3 (c)
		start = i
		for i < len(row) && row[i] != ',' {
			i++
		}
		c := parseIntFast(row[start:i])
		i++ // skip comma
		
		// Check field 4 for "hot"
		// Field 4 starts at i, ends at next comma or end of string
		hot := false
		if i+2 < len(row) && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
			// Check it's exactly "hot" (followed by comma or end)
			if i+3 >= len(row) || row[i+3] == ',' {
				hot = true
			}
		}
		
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseIntFast(s string) int {
	if len(s) == 0 {
		return 0
	}
	neg := false
	i := 0
	if s[0] == '-' {
		neg = true
		i = 1
	} else if s[0] == '+' {
		i = 1
	}
	n := 0
	for ; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}