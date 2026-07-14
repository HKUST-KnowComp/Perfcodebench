package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		i := 0
		commas := 0
		b, c := 0, 0
		cur := 0
		neg := false

		for i < n {
			ch := row[i]
			if ch == ',' {
				if commas == 2 {
					if neg {
						b = -cur
					} else {
						b = cur
					}
					cur = 0
					neg = false
				} else if commas == 3 {
					if neg {
						c = -cur
					} else {
						c = cur
					}
					cur = 0
					neg = false
				}
				commas++
				i++
				continue
			}
			if commas == 4 {
				break
			}
			if commas == 2 || commas == 3 {
				if ch == '-' {
					neg = true
				} else {
					cur = cur*10 + int(ch-'0')
				}
			}
			i++
		}
		// Handle case where c is at the end without a trailing comma
		if commas == 3 {
			if neg {
				c = -cur
			} else {
				c = cur
			}
		}

		hot := i+2 < n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}