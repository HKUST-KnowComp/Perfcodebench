package main

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	hash := uint64(fnvOffset)

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// single pass over the row
		i := 0
		n := len(row)
		for i < n {
			// find next key start
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote

			// identify field name
			start := i
			for i < n && row[i] != '"' {
				i++
			}
			field := row[start:i]
			i++ // skip closing quote

			// skip colon
			for i < n && row[i] != ':' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip colon

			// skip optional whitespace
			for i < n && (row[i] == ' ' || row[i] == '\t') {
				i++
			}

			// parse value based on field
			switch field {
			case "user":
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				user = v
			case "score":
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				score = v
			case "flag":
				if i < n && row[i] == 't' {
					flag = true
					// skip "true"
					i += 4
				} else {
					// skip "false"
					i += 5
				}
			default:
				// skip uninteresting value
				depth := 0
				inStr := false
				for i < n {
					ch := row[i]
					i++
					if inStr {
						if ch == '"' && row[i-2] != '\\' {
							inStr = false
						}
						continue
					}
					switch ch {
					case '"':
						inStr = true
					case '{', '[':
						depth++
					case '}', ']':
						depth--
						if depth < 0 {
							goto nextField
						}
					case ',':
						if depth == 0 {
							goto nextField
						}
					}
				}
			nextField:
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