package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find field boundaries by locating commas
		// Fields: 0, 1, 2 (b), 3 (c), 4 (hot)
		n := len(row)
		i := 0
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
		// Check field 4 for "hot"
		hot := (n-i >= 3) && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'
		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
