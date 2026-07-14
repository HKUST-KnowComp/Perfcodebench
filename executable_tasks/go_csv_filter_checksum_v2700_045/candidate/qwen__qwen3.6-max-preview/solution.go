package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		hot := false
		field := 0
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				if field == 2 {
					b = fastAtoi(row[start:i])
				} else if field == 3 {
					c = fastAtoi(row[start:i])
				} else if field == 4 {
					hot = i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
				}
				field++
				start = i + 1
				if field > 4 {
					break
				}
			}
		}
		if field == 4 {
			end := len(row)
			hot = end-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
		}

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func fastAtoi(s string) int {
	var n int
	neg := false
	if len(s) > 0 && s[0] == '-' {
		neg = true
		s = s[1:]
	}
	for i := 0; i < len(s); i++ {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		n = n*10 + int(c-'0')
	}
	if neg {
		return -n
	}
	return n
}