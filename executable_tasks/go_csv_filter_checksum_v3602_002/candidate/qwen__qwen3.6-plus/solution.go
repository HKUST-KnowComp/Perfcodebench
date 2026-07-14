package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 0
		n := len(row)

		// Skip field 0
		for i < n && row[i] != ',' {
			i++
		}
		i++
		// Skip field 1
		for i < n && row[i] != ',' {
			i++
		}
		i++

		// Parse field 2 (b)
		b := 0
		for i < n && row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		i++

		// Parse field 3 (c)
		c := 0
		for i < n && row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		i++

		// Check field 4 (hot)
		hot := false
		if i+3 <= n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
			if i+3 == n || row[i+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
