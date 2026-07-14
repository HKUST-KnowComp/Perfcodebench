package main

func run(rows []string) uint64 {
	const (
		offsetPrime = 1469598103934665603
		prime      = 1099511628211
	)
	hash := uint64(offsetPrime)

	for _, row := range rows {
		b := 0
		c := 0
		i := 0

		// field 0
		for row[i] != ',' {
			i++
		}
		i++ // skip ','

		// field 1
		for row[i] != ',' {
			i++
		}
		i++ // skip ','

		// field 2 (int)
		for row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		i++ // skip ','

		// field 3 (int)
		for row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		i++ // skip ','

		// field 4 (flag)
		hot := row[i] == 'h' // "hot" starts with 'h'

		v := uint64(b+c+11)
		if hot {
			v += 29
		}

		hash ^= v + 1
		hash *= prime
	}
	return hash
}