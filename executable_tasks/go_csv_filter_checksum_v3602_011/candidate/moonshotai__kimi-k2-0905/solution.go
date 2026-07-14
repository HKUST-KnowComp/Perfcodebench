package main

func run(rows []string) uint64 {
	const prime = 1099511628211
	hash := uint64(1469598103934665603)

	// scratch buffer reused for every row
	scratch := make([]byte, 0, 32)

	for _, row := range rows {
		b := 0
		c := 0
		isHot := false

		// Skip first field (up to first comma)
		i := 0
		for row[i] != ',' {
			i++
		}
		i++ // move past first comma

		// Skip second field (up to second comma)
		for row[i] != ',' {
			i++
		}
		i++ // move past second comma

		// Parse third field (b)
		for row[i] != ',' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		i++ // move past third comma

		// Parse fourth field (c)
		for row[i] != ',' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		i++ // move past fourth comma

		// Check fifth field for "hot"
		isHot = row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'

		v := uint64(b + c + 23)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime

		// reuse scratch for next row
		scratch = scratch[:0]
	}
	return hash
}