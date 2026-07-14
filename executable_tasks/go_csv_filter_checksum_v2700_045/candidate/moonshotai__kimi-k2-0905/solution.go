package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211
	hash := uint64(fnvOffset)

	for _, row := range rows {
		b := 0
		c := 0
		isHot := false

		// skip first field
		s := 0
		for row[s] != ',' { s++ }
		s++ // comma

		// second field → b
		for row[s] != ',' {
			b = b*10 + int(row[s]-'0')
			s++
		}
		s++ // comma

		// third field → c
		for row[s] != ',' {
			c = c*10 + int(row[s]-'0')
			s++
		}
		s++ // comma
s++}

		// fifth field → "hot" flag
		// we already skipped the 4th field above by advancing past the 3rd comma.
		// so now we are at the 5th field.
		if row[s] == 'h' && row[s+1] == 'o' && row[s+2] == 't' {
			isHot = true
		}

		v := uint64(b + c + 29)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}