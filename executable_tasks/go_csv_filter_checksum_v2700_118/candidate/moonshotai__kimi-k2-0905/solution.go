package main

func run(rows []string) uint64 {
	const (
		prime = 1099511628211
		offset = 1469598103934665603
	)
	hash := offset
	for _, row := range rows {
		// find 3rd field (index 2)
		i, commas := 0, 0
		for ; i < len(row); i++ {
			if row[i] == ',' {
				commas++
				if commas == 2 {
					i++
					break
				}
			}
		}
		// parse b
		b := 0
		for ; i < len(row) && row[i] != ','; i++ {
			b = b*10 + int(row[i]-'0')
		}
		i++ // skip comma
		// parse c
		c := 0
		for ; i < len(row) && row[i] != ','; i++ {
			c = c*10 + int(row[i]-'0')
		}
		i++ // skip comma
		// check hot flag
		hot := i < len(row)-3 && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'
		v := uint64(b+c+10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}