package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	hash := uint64(fnvOffset)

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// single pass over the line
		i := 0
		n := len(row)
		for i < n {
			// locate start of next key
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote

			keyStart := i
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			keyLen := i - keyStart
			i++ // skip closing quote

			// expect ':'
			for i < n && (row[i] == ' ' || row[i] == ':') {
				i++
			}
			if i >= n {
				break
			}

			valueStart := i
			switch {
			case keyLen == 4 && row[keyStart] == 'u' && row[keyStart+1] == 's' && row[keyStart+2] == 'e' && row[keyStart+3] == 'r':
				for valueStart < n && row[valueStart] != '"' {
					valueStart++
				}
				if valueStart < n {
					valueStart++ // skip opening quote
					v := 0
					for valueStart < n && row[valueStart] != '"' {
						c := row[valueStart]
						if c >= '0' && c <= '9' {
							v = v*10 + int(c-'0')
						}
						valueStart++
					}
					user = v
				}
			case keyLen == 5 && row[keyStart] == 's' && row[keyStart+1] == 'c' && row[keyStart+2] == 'o' && row[keyStart+3] == 'r' && row[keyStart+4] == 'e':
				for valueStart < n && row[valueStart] != '"' {
					valueStart++
				}
				if valueStart < n {
					valueStart++ // skip opening quote
					v := 0
					for valueStart < n && row[valueStart] != '"' {
						c := row[valueStart]
						if c >= '0' && c <= '9' {
							v = v*10 + int(c-'0')
						}
						valueStart++
					}
					score = v
				}
			case keyLen == 4 && row[keyStart] == 'f' && row[keyStart+1] == 'l' && row[keyStart+2] == 'a' && row[keyStart+3] == 'g':
				skipSpace := valueStart
				for skipSpace < n && (row[skipSpace] == ' ' || row[skipSpace] == '\t') {
					skipSpace++
				}
				if skipSpace+3 < n && row[skipSpace] == 't' && row[skipSpace+1] == 'r' && row[skipSpace+2] == 'u' && row[skipSpace+3] == 'e' {
					flag = true
				}
			}

			// advance to next key
			for i < n && row[i] != ',' && row[i] != '}' {
				i++
			}
			if i < n && row[i] == ',' {
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

// local helper renamed to avoid clash with other packages
const eleStart = ' '
const eleEnd   = ' '}
