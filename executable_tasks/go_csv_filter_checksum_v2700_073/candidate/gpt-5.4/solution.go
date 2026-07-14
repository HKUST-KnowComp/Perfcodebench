package main

func run(rows []string) uint64 {
	const (
		offset64 = 1469598103934665603
		prime64  = 1099511628211
	)

	hash := uint64(offset64)
	for _, row := range rows {
		field := 0
		val := 0
		neg := false
		haveNum := false
		hot := false
		b, c := 0, 0

		for i := 0; i < len(row); i++ {
			ch := row[i]
			if ch == ',' {
				if field == 2 {
					if neg {
						val = -val
					}
					b = val
				} else if field == 3 {
					if neg {
						val = -val
					}
					c = val
				}
				field++
				val = 0
				neg = false
				haveNum = false
				continue
			}

			switch field {
			case 2, 3:
				if ch == '-' && !haveNum {
					neg = true
				} else {
					val = val*10 + int(ch-'0')
					haveNum = true
				}
			case 4:
				// parts[4] == "hot"
				if i+2 < len(row) && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
					hot = true
				}
				// No need to scan remaining bytes for this row.
				i = len(row)
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
