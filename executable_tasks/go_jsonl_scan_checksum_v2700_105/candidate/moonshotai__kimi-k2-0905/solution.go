package main

func run(rows []string) uint64 {
	const (
		offset64 = 1469598103934665603
		prime64  = 1099511628211
	)
	hash := uint64(offset64)

	for _, row := range rows {
		b := []byte(row)
		user, score := 0, 0
		flag := false

		// single pass over []byte
		for i := 0; i < len(b); {
			// fast skip to next key
			for i < len(b) && b[i] != '"' {
				i++
			}
			if i >= len(b) {
				break
			}
			i++ // skip "

			// key start
			s := i
			for i < len(b) && b[i] != '"' {
				i++
			}
			key := b[s:i]
			if i >= len(b) {
				break
			}
			i++ // skip closing "

			// expect ':'
			for i < len(b) && (b[i] == ' ' || b[i] == '\t') {
				i++
			}
			if i >= len(b) || b[i] != ':' {
				continue
			}
			i++ // skip :

			// skip spaces
			for i < len(b) && (b[i] == ' ' || b[i] == '\t') {
				i++
			}
			if i >= len(b) {
				break
			}

			switch {
			case len(key) == 4 && key[0] == 'u' && key[1] == 's' && key[2] == 'e' && key[3] == 'r':
				// parse integer
				neg := false
				if b[i] == '-' {
					neg = true
					i++
				}
				for i < len(b) && b[i] >= '0' && b[i] <= '9' {
					user = user*10 + int(b[i]-'0')
					i++
				}
				if neg {
					user = -user
				}
			case len(key) == 5 && key[0] == 's' && key[1] == 'c' && key[2] == 'o' && key[3] == 'r' && key[4] == 'e':
				// parse integer
				neg := false
				if b[i] == '-' {
					neg = true
					i++
				}
				for i < len(b) && b[i] >= '0' && b[i] <= '9' {
					score = score*10 + int(b[i]-'0')
					i++
				}
				if neg {
					score = -score
				}
			case len(key) == 4 && key[0] == 'f' && key[1] == 'l' && key[2] == 'a' && key[3] == 'g':
				if i+3 < len(b) && b[i] == 't' && b[i+1] == 'r' && b[i+2] == 'u' && b[i+3] == 'e' {
					flag = true
					i += 4
				} else if i+4 < len(b) && b[i] == 'f' && b[i+1] == 'a' && b[i+2] == 'l' && b[i+3] == 's' && b[i+4] == 'e' {
					flag = false
					i += 5
				}
			default:
				// skip value
				if b[i] == '"' {
					i++ // skip opening quote
					for i < len(b) && b[i] != '"' {
						if b[i] == '\\' && i+1 < len(b) {
							i += 2
						} else {
							i++
						}
					}
					if i < len(b) {
						i++ // skip closing quote
					}
				} else {
					for i < len(b) && b[i] != ',' && b[i] != '}' {
						i++
					}
				}
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}