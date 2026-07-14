package main

func run(rows []string) uint64 {
	const (
		offset = 1469598103934665603
		prime  = 1099511628211
	)
	hash := offset
	for _, row := range rows {
		b := 0
		c := 0
		comma := 0
		// field 0
		for i := 0; row[i] != ','; i++ {
		}
		comma++
		// field 1
		for i := 0; row[comma+i] != ','; i++ {
		}
		comma++
		// field 2 -> b
		p := comma
		for row[p] != ',' {
			b = b*10 + int(row[p]-'0')
			p++
		}
		comma = p + 1
		// field 3 -> c
		p = comma
		for row[p] != ',' {
			c = c*10 + int(row[p]-'0')
			p++
		}
		comma = p + 1
		// field 4 -> hot check
		hot := row[comma] == 'h' && row[comma+1] == 'o' && row[comma+2] == 't'
		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}