package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// single pass byte scanner
		i := 0
		for i < len(row) {
			// skip commas and whitespace (but input likely no whitespace, but safe)
			for i < len(row) && (row[i] == ',' || row[i] == ' ') {
				i++
			}
			if i >= len(row) {
				break
			}

			// expect double quote to start a key
			if row[i] != '"' {
				// malformed, skip to next comma
				for i < len(row) && row[i] != ',' {
					i++
				}
				continue
			}
			i++ // skip opening quote
			// read key until closing quote
			keyStart := i
			for i < len(row) && row[i] != '"' {
				i++
			}
			if i >= len(row) {
				break
			}
			// key is row[keyStart:i]
			keyLen := i - keyStart
			i++ // skip closing quote

			// expect colon
			for i < len(row) && (row[i] == ':' || row[i] == ' ') {
				i++
			}
			if i >= len(row) {
				break
			}

			// now parse value based on key
			if keyLen == 4 && row[keyStart] == 'u' && row[keyStart+1] == 's' && row[keyStart+2] == 'e' && row[keyStart+3] == 'r' {
				// parse number until comma or end
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if keyLen == 5 && row[keyStart] == 's' && row[keyStart+1] == 'c' && row[keyStart+2] == 'o' && row[keyStart+3] == 'r' && row[keyStart+4] == 'e' {
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if keyLen == 4 && row[keyStart] == 'f' && row[keyStart+1] == 'l' && row[keyStart+2] == 'a' && row[keyStart+3] == 'g' {
				// expect "true" or "false"
				if i+4 <= len(row) && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
					flag = true
					i += 4
				} else {
					// skip false or unknown
					for i < len(row) && row[i] != ',' && row[i] != '}' && row[i] != '"' {
						i++
					}
				}
			} else {
				// unknown key, skip value
				for i < len(row) && row[i] != ',' && row[i] != '}' && row[i] != '"' {
					i++
				}
			}
			// after value, expect comma or end
			for i < len(row) && (row[i] == ',' || row[i] == ' ') {
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
