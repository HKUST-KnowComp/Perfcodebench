package main

func run(rows []string) uint64 {
	const (
		offsetBasis = 1469598103934665603
		prime       = 1099511628211
	)
	hash := offsetBasis

	for _, row := range rows {
		b := 0
		c := 0
		comma := 0
		pos := 0

		// walk to 2nd comma (field index 2)
		for comma < 2 && pos < len(row) {
			if row[pos] == ',' {
				comma++
			}
			pos++
		}

		// parse field 2
		for pos < len(row) && row[pos] >= '0' && row[pos] <= '9' {
			b = b*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip comma

		// parse field 3
		for pos < len(row) && row[pos] >= '0' && row[pos] <= '9' {
			c = c*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip comma

		// decide hot flag
		hot := pos+3 <= len(row) &&
			row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't'

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}

		hash ^= v + 1
		hash *= prime
	}
	return hash
}