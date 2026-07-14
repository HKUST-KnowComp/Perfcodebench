package main

// The spec guarantees the input format: "a,b,c,d,hot|cold"
// We only need the 3rd and 4th field and the 5th flag.
func run(rows []string) uint64 {
	hash := uint64(1469598103934665603) // FNV-64 offset basis

	for _, row := range rows {
		start := 0
		// field 3 (index 2)
		for i := 0; i < 2; i++ {
			idx := 0
			for j := start; j < len(row); j++ {
				if row[j] == ',' {
					idx++
					if idx == i+1 {
						start = j + 1
						break
					}
				}
			}
		}
		// parse field 3 (b)
		b := 0
		for i := start; i < len(row); i++ {
			if row[i] == ',' {
				start = i + 1
				break
			}
			b = b*10 + int(row[i]-'0')
		}
		// parse field 4 (c)
		c := 0
		for i := start; i < len(row); i++ {
			if row[i] == ',' {
				start = i + 1
				break
			}
			c = c*10 + int(row[i]-'0')
		}
		// check field 5 for "hot"
		hot := row[start:start+3] == "hot"

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211 // FNV-64 prime
	}
	return hash
}