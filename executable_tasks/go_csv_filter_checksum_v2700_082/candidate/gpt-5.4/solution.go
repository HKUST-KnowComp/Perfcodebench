package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for i := 0; i < len(rows); i++ {
		s := rows[i]
		field := 0
		val := 0
		b := 0
		c := 0
		hot := false

		for j := 0; j < len(s); j++ {
			ch := s[j]
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

			if field == 2 || field == 3 {
				val = val*10 + int(ch-'0')
			} else if field == 4 {
				// Compare against "hot" without slicing or allocation.
				if j+2 < len(s) && s[j] == 'h' && s[j+1] == 'o' && s[j+2] == 't' {
					hot = true
				}
				break
			}
		}

		// Handle rows where field 3 reaches end without trailing comma.
		if field == 3 {
			c = val
		}

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}

	return hash
}
