package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		pos := 0

		// Skip field 0
		for pos < n && row[pos] != ',' {
			pos++
		}
		pos++
		// Skip field 1
		for pos < n && row[pos] != ',' {
			pos++
		}
		pos++

		// Parse field 2 (b)
		b := 0
		for pos < n && row[pos] != ',' {
			b = b*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Parse field 3 (c)
		c := 0
		for pos < n && row[pos] != ',' {
			c = c*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Check field 4 (hot)
		hot := pos+3 <= n && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't'

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}