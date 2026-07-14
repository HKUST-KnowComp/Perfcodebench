package main

func run(rows []string) uint64 {
	const prime = 1099511628211
	const offset = 1469598103934665603
	hash := offset
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		i := 0
		n := len(row)
		for i < n {
			// skip until a quote (start of key)
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote

			// extract key
			keyStart := i
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			key := row[keyStart:i]
			i++ // skip closing quote

			// skip to colon
			for i < n && row[i] != ':' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip colon

			// skip whitespace after colon (optional, but safe)
			for i < n && (row[i] == ' ' || row[i] == '\t') {
				i++
			}

			// parse value based on key
			if len(key) == 4 && key[0] == 'u' && key[1] == 's' && key[2] == 'e' && key[3] == 'r' {
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if len(key) == 5 && key[0] == 's' && key[1] == 'c' && key[2] == 'o' && key[3] == 'r' && key[4] == 'e' {
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if len(key) == 4 && key[0] == 'f' && key[1] == 'l' && key[2] == 'a' && key[3] == 'g' {
				// expect "true" or "false"
				if i+4 <= n && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
					flag = true
					i += 4
				} else {
					// skip "false" or unknown
					for i < n && row[i] != ',' {
						i++
					}
				}
			} else {
				// unknown key, skip until comma
				for i < n && row[i] != ',' {
					i++
				}
			}

			// skip comma
			if i < n && row[i] == ',' {
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
