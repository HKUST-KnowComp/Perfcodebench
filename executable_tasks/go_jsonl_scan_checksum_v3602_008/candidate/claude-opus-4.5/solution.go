package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		n := len(row)
		for i := 0; i < n; {
			c := row[i]
			if c == '"' && i+7 < n && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				i += 7
				for i < n {
					d := row[i]
					if d >= '0' && d <= '9' {
						user = user*10 + int(d-'0')
						i++
					} else if d == ',' || d == '}' {
						break
					} else {
						i++
					}
				}
			} else if c == '"' && i+8 < n && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				i += 8
				for i < n {
					d := row[i]
					if d >= '0' && d <= '9' {
						score = score*10 + int(d-'0')
						i++
					} else if d == ',' || d == '}' {
						break
					} else {
						i++
					}
				}
			} else if c == '"' && i+12 < n && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' && row[i+8] == 'r' && row[i+9] == 'u' && row[i+10] == 'e' {
				flag = true
				i += 11
			} else {
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