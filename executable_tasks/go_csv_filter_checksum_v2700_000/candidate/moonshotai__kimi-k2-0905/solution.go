package main

func run(rows []string) uint64 {
	const prime = 1099511628211
	hash := uint64(1469598103934665603)

	for _, s := range rows {
		b := 0
		c := 0
		comma := 0

		// field 2 (index 2)
		for i := 0; i < len(s); i++ {
			if s[i] == ',' {
				comma++
				if comma == 2 {
					i++
					for ; i < len(s) && s[i] >= '0' && s[i] <= '9'; i++ {
						b = b*10 + int(s[i]-'0')
					}
					break
				}
			}
		}

		// field 3 (index 3)
		for i := 0; i < len(s); i++ {
			if s[i] == ',' {
				comma++
				if comma == 3 {
					i++
					for ; i < len(s) && s[i] >= '0' && s[i] <= '9'; i++ {
						c = c*10 + int(s[i]-'0')
					}
					break
				}
			}
		}

		// field 4 (index 4) -> "hot"
		hot := false
		for i := 0; i < len(s); i++ {
			if s[i] == ',' {
				comma++
				if comma == 4 {
					if i+4 < len(s) && s[i+1] == 'h' && s[i+2] == 'o' && s[i+3] == 't' && (i+4 == len(s) || s[i+4] == ',') {
						hot = true
					}
					break
				}
			}
		}

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}