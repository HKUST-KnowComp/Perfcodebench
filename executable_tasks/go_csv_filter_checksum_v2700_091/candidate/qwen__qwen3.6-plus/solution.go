package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 0
		// Skip first two fields
		for row[i] != ',' { i++ }
		i++
		for row[i] != ',' { i++ }
		i++
		
		// Parse field 2 (b)
		b := 0
		neg := false
		if row[i] == '-' {
			neg = true
			i++
		}
		for row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		if neg { b = -b }
		i++
		
		// Parse field 3 (c)
		c := 0
		neg = false
		if row[i] == '-' {
			neg = true
			i++
		}
		for row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		if neg { c = -c }
		i++
		
		// Check field 4 for "hot"
		hot := row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' && (i+3 == len(row) || row[i+3] == ',')
		
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}