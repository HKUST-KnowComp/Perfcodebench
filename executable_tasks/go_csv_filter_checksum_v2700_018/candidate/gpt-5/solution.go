package main

func run(rows []string) uint64 {
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		b := 0
		c := 0
		hot := false

		field := 0
		i := 0
		n := len(row)
		for {
			start := i
			for i < n && row[i] != ',' {
				i++
			}

			if field == 2 {
				// parse integer b
				sign := 1
				j := start
				if j < i && row[j] == '-' {
					sign = -1
					j++
				}
				val := 0
				for ; j < i; j++ {
					val = val*10 + int(row[j]-'0')
				}
				b = sign * val
			} else if field == 3 {
				// parse integer c
				sign := 1
				j := start
				if j < i && row[j] == '-' {
					sign = -1
					j++
				}
				val := 0
				for ; j < i; j++ {
					val = val*10 + int(row[j]-'0')
				}
				c = sign * val
			} else if field == 4 {
				// check if equals "hot"
				if i-start == 3 {
					if row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
				}
			}

			if i == n {
				field++
				break
			}
			// skip comma
			i++
			field++
			if field > 4 {
				break
			}
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}
