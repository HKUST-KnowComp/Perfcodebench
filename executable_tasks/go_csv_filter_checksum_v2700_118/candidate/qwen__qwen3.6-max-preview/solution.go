package main

func fastAtoi(s string) int {
	if len(s) == 0 {
		return 0
	}
	neg := false
	i := 0
	if s[0] == '-' {
		neg = true
		i = 1
	} else if s[0] == '+' {
		i = 1
	}
	var n int
	for ; i < len(s); i++ {
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

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		commaCount := 0
		start := 0
		var b, c int
		var hot bool

		for i := 0; i < n; i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 3 {
					b = fastAtoi(row[start:i])
				} else if commaCount == 4 {
					c = fastAtoi(row[start:i])
				} else if commaCount == 5 {
					hot = row[start:i] == "hot"
					break
				}
				start = i + 1
			}
		}
		if commaCount == 4 {
			hot = row[start:] == "hot"
		}

		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
