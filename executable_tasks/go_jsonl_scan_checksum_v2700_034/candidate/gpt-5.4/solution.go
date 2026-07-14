package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		user, score := 0, 0
		flag := false

		i := 0
		n := len(row)
		for i < n {
			if row[i] != '"' {
				i++
				continue
			}

			// Fast-path known keys in these JSON-like rows.
			if i+7 <= n && row[i:i+6] == "\"user\"" {
				j := i + 6
				for j < n && row[j] != ':' {
					j++
				}
				if j < n {
					j++
					for j < n && (row[j] == ' ' || row[j] == '\t') {
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
					user = v * sign
					i = j
					continue
				}
			} else if i+8 <= n && row[i:i+7] == "\"score\"" {
				j := i + 7
				for j < n && row[j] != ':' {
					j++
				}
				if j < n {
					j++
					for j < n && (row[j] == ' ' || row[j] == '\t') {
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
					score = v * sign
					i = j
					continue
				}
			} else if i+7 <= n && row[i:i+6] == "\"flag\"" {
				j := i + 6
				for j < n && row[j] != ':' {
					j++
				}
				if j < n {
					j++
					for j < n && (row[j] == ' ' || row[j] == '\t') {
						j++
					}
					if j+4 <= n && row[j:j+4] == "true" {
						flag = true
					} else {
						flag = false
					}
					i = j
					continue
				}
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
