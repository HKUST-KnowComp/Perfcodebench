package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		n := len(row)
		for i := 0; i < n; i++ {
			if row[i] != '"' {
				continue
			}

			// Match "user":
			if i+7 <= n && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				j := i + 7
				v := 0
				for j < n {
					c := row[j]
					if c >= '0' && c <= '9' {
						v = v*10 + int(c-'0')
						j++
						for j < n {
							c = row[j]
							if c < '0' || c > '9' {
								break
							}
							v = v*10 + int(c-'0')
							j++
						}
						break
					}
					if c == ',' || c == '}' {
						break
					}
					j++
				}
				user = v
				i = j
				continue
			}

			// Match "score":
			if i+8 <= n && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				j := i + 8
				v := 0
				for j < n {
					c := row[j]
					if c >= '0' && c <= '9' {
						v = v*10 + int(c-'0')
						j++
						for j < n {
							c = row[j]
							if c < '0' || c > '9' {
								break
							}
							v = v*10 + int(c-'0')
							j++
						}
						break
					}
					if c == ',' || c == '}' {
						break
					}
					j++
				}
				score = v
				i = j
				continue
			}

			// Match "flag":true
			if i+11 <= n && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' && row[i+8] == 'r' && row[i+9] == 'u' && row[i+10] == 'e' {
				flag = true
				i += 10
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
