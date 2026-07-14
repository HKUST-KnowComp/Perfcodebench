package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		n := len(row)
		i := 0

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

		// Parse b (field 2)
		neg := false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		b := 0
		for i < n && row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		if neg {
			b = -b
		}
		i++

		// Parse c (field 3)
		neg = false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		c := 0
		for i < n && row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		if neg {
			c = -c
		}
		i++

		// Check "hot" (field 4) exactly
		hot := i+3 <= n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' && (i+3 == n || row[i+3] == ',')

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
