package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4
		// Format: f0,f1,f2,f3,f4,...
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
		for i < n && row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		i++ // skip comma

		// Parse field 3 (c)
		c := 0
		for i < n && row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		i++ // skip comma

		// Check field 4 for "hot"
		hot := false
		if i+2 < n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
			if i+3 >= n || row[i+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
