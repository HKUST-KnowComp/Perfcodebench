package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Single-pass scan for the three known keys.
		for i := 0; i < len(row); i++ {
			if row[i] != '"' {
				continue
			}

			// Match "user"
			if i+6 < len(row) && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' {
				j := i + 6
				for j < len(row) && row[j] != ':' {
					j++
				}
				if j < len(row) {
					j++
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
				}
				continue
			}

			// Match "score"
			if i+7 < len(row) && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' {
				j := i + 7
				for j < len(row) && row[j] != ':' {
					j++
				}
				if j < len(row) {
					j++
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
				}
				continue
			}

			// Match "flag"
			if i+6 < len(row) && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' {
				j := i + 6
				for j < len(row) && row[j] != ':' {
					j++
				}
				if j < len(row) {
					j++
					for j < len(row) && (row[j] == ' ' || row[j] == '\t') {
						j++
					}
					if j+3 < len(row) && row[j] == 't' && row[j+1] == 'r' && row[j+2] == 'u' && row[j+3] == 'e' {
						flag = true
					}
				}
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
