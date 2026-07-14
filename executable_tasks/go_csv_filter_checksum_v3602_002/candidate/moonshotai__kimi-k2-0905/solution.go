package main

func run(rows []string) uint64 {
	const (
		prime = 1099511628211
		offset = 1469598103934665603
	)
	hash := offset
	for _, row := range rows {
		b := 0
		c := 0
		s := []byte(row)
		// skip first field
		i := 0
		for i < len(s) && s[i] != ',' {
			i++
		}
		i++ // comma
		// skip second field
		for i < len(s) && s[i] != ',' {
			i++
		}
		i++ // comma
		// parse third field (b)
		for i < len(s) && s[i] != ',' {
			b = b*10 + int(s[i]-'0')
			i++
		}
		i++ // comma
		// parse fourth field (c)
		for i < len(s) && s[i] != ',' {
			c = c*10 + int(s[i]-'0')
			i++
		}
		i++ // comma
		// check fifth field == "hot"
		hot := i+3 <= len(s) && s[i] == 'h' && s[i+1] == 'o' && s[i+2] == 't'
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}