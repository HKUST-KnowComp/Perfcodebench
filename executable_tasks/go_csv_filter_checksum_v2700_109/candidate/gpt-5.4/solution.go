package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		val := 0
		b := 0
		c := 0
		hot := false

		for i := 0; i < len(row); i++ {
			ch := row[i]
			if ch == ',' {
				switch field {
				case 2:
					b = val
				case 3:
					c = val
				}
				field++
				val = 0
				continue
			}

			switch field {
			case 2, 3:
				val = val*10 + int(ch-'0')
			case 4:
				// parts[4] == "hot"
				if i+2 < len(row) && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
					hot = true
				}
				i = len(row)
			}
		}

		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
