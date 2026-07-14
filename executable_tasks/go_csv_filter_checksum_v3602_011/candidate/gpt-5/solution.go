package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var h uint64 = offset

	for _, s := range rows {
		n := len(s)
		i := 0

		// Skip first field
		for i < n && s[i] != ',' {
			i++
		}
		if i < n {
			i++
		}
		// Skip second field
		for i < n && s[i] != ',' {
			i++
		}
		if i < n {
			i++
		}

		// Parse third field (b)
		b := 0
		if i < n {
			sign := 1
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
			vb := 0
			for i < n {
				ch := s[i]
				if ch == ',' {
					break
				}
				vb = vb*10 + int(ch-'0')
				i++
			}
			b = sign * vb
		}
		if i < n && s[i] == ',' {
			i++
		}

		// Parse fourth field (c)
		cVal := 0
		if i < n {
			sign := 1
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
			vc := 0
			for i < n {
				ch := s[i]
				if ch == ',' {
					break
				}
				vc = vc*10 + int(ch-'0')
				i++
			}
			cVal = sign * vc
		}
		if i < n && s[i] == ',' {
			i++
		}

		// Check fifth field equals "hot"
		start := i
		for i < n && s[i] != ',' {
			i++
		}
		hot := false
		if i-start == 3 && s[start] == 'h' && s[start+1] == 'o' && s[start+2] == 't' {
			hot = true
		}

		sum := b + cVal + 23
		if hot {
			sum += 29
		}
		v := uint64(sum)
		h ^= v + 1
		h *= prime
	}
	return h
}
