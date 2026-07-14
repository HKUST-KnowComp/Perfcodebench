package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		n := len(row)
		i := 0
		for i < n {
			if row[i] != '"' {
				i++
				continue
			}

			// Match known keys directly to avoid allocations and repeated scans.
			if i+6 < n && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' {
				j := i + 6
				for j < n {
					c := row[j]
					if c == ':' {
						j++
						break
					}
					j++
				}
				for j < n {
					c := row[j]
					if c != ' ' && c != '\t' && c != '\n' && c != '\r' {
						break
					}
					j++
				}
				sign := 1
				if j < n && row[j] == '-' {
					sign = -1
					j++
				}
				v := 0
				for j < n {
					c := row[j]
					if c < '0' || c > '9' {
						break
					}
					v = v*10 + int(c-'0')
					j++
				}
				user = sign * v
				i = j
				continue
			}

			if i+7 < n && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' {
				j := i + 7
				for j < n {
					c := row[j]
					if c == ':' {
						j++
						break
					}
					j++
				}
				for j < n {
					c := row[j]
					if c != ' ' && c != '\t' && c != '\n' && c != '\r' {
						break
					}
					j++
				}
				sign := 1
				if j < n && row[j] == '-' {
					sign = -1
					j++
				}
				v := 0
				for j < n {
					c := row[j]
					if c < '0' || c > '9' {
						break
					}
					v = v*10 + int(c-'0')
					j++
				}
				score = sign * v
				i = j
				continue
			}

			if i+6 < n && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' {
				j := i + 6
				for j < n {
					c := row[j]
					if c == ':' {
						j++
						break
					}
					j++
				}
				for j < n {
					c := row[j]
					if c != ' ' && c != '\t' && c != '\n' && c != '\r' {
						break
					}
					j++
				}
				if j+3 < n && row[j] == 't' && row[j+1] == 'r' && row[j+2] == 'u' && row[j+3] == 'e' {
					flag = true
				}
				i = j
				continue
			}

			i++
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
