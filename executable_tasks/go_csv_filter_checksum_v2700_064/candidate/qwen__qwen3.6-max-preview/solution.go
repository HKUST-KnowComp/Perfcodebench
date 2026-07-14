package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		idx := 0
		n := len(row)

		// Skip field 0
		for idx < n && row[idx] != ',' {
			idx++
		}
		if idx < n {
			idx++
		}

		// Skip field 1
		for idx < n && row[idx] != ',' {
			idx++
		}
		if idx < n {
			idx++
		}

		// Parse field 2 (b)
		start := idx
		for idx < n && row[idx] != ',' {
			idx++
		}
		b := 0
		neg := false
		if start < idx && row[start] == '-' {
			neg = true
			start++
		}
		for i := start; i < idx; i++ {
			b = b*10 + int(row[i]-'0')
		}
		if neg {
			b = -b
		}
		if idx < n {
			idx++
		}

		// Parse field 3 (c)
		start = idx
		for idx < n && row[idx] != ',' {
			idx++
		}
		c := 0
		neg = false
		if start < idx && row[start] == '-' {
			neg = true
			start++
		}
		for i := start; i < idx; i++ {
			c = c*10 + int(row[i]-'0')
		}
		if neg {
			c = -c
		}
		if idx < n {
			idx++
		}

		// Check field 4 (hot)
		start = idx
		for idx < n && row[idx] != ',' {
			idx++
		}
		hot := row[start:idx] == "hot"

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
