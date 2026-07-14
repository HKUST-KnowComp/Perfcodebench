package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	var hash uint64 = offset64

	for _, row := range rows {
		var user, score int
		flag := false

		// Single-pass scan for the three known keys.
		for i := 0; i < len(row); i++ {
			switch row[i] {
			case 'u':
				// Match: "user":
				if i >= 1 && row[i-1] == '"' && i+6 < len(row) &&
					row[i+1] == 's' && row[i+2] == 'e' && row[i+3] == 'r' &&
					row[i+4] == '"' && row[i+5] == ':' {
					j := i + 6
					for j < len(row) {
						c := row[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
							j++
							for j < len(row) {
								c = row[j]
								if c < '0' || c > '9' {
									break
								}
								user = user*10 + int(c-'0')
								j++
							}
							break
						}
						j++
					}
					i = j
				}
			case 's':
				// Match: "score":
				if i >= 1 && row[i-1] == '"' && i+7 < len(row) &&
					row[i+1] == 'c' && row[i+2] == 'o' && row[i+3] == 'r' && row[i+4] == 'e' &&
					row[i+5] == '"' && row[i+6] == ':' {
					j := i + 7
					for j < len(row) {
						c := row[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
							j++
							for j < len(row) {
								c = row[j]
								if c < '0' || c > '9' {
									break
								}
								score = score*10 + int(c-'0')
								j++
							}
							break
						}
						j++
					}
					i = j
				}
			case 'f':
				// Match: "flag":true
				if i >= 1 && row[i-1] == '"' && i+9 < len(row) &&
					row[i+1] == 'l' && row[i+2] == 'a' && row[i+3] == 'g' &&
					row[i+4] == '"' && row[i+5] == ':' &&
					row[i+6] == 't' && row[i+7] == 'r' && row[i+8] == 'u' && row[i+9] == 'e' {
					flag = true
					i += 9
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
