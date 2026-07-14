package main

func run(rows []string) uint64 {
	const (
		offset64 = 1469598103934665603
		prime64  = 1099511628211
	)
	hash := uint64(offset64)

	for _, row := range rows {
		b := 0
		c := 0
		comma := 0
		// skip first two commas
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				comma++
				if comma == 2 {
					// start of field 2 (b)
					i++
					for ; i < len(row) && row[i] != ','; i++ {
						b = b*10 + int(row[i]-'0')
					}
					// skip comma
					i++
					// field 3 (c)
					for ; i < len(row) && row[i] != ','; i++ {
						c = c*10 + int(row[i]-'0')
					}
					// skip comma
					i++
					// field 4 hot flag
					hot := i+3 <= len(row) &&
						row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'
					v := uint64(b + c + 24)
					if hot {
						v += 29
					}
					hash ^= v + 1
					hash *= prime64
					break
				}
			}
		}
	}
	return hash
}