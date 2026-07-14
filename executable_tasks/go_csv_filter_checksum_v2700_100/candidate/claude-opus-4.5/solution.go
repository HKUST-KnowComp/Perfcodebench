package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4
		// Format: field0,field1,field2,field3,field4,...
		// We need parts[2], parts[3], parts[4]
		
		i := 0
		n := len(row)
		
		// Skip field 0
		for i < n && row[i] != ',' {
			i++
		}
		i++ // skip comma
		
		// Skip field 1
		for i < n && row[i] != ',' {
			i++
		}
		i++ // skip comma
		
		// Parse field 2 (b)
		b := 0
		neg := false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		for i < n && row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		if neg {
			b = -b
		}
		i++ // skip comma
		
		// Parse field 3 (c)
		c := 0
		neg = false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		for i < n && row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		if neg {
			c = -c
		}
		i++ // skip comma
		
		// Check field 4 == "hot"
		// "hot" is 3 chars: 'h', 'o', 't'
		hot := i+2 < n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'
		
		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}