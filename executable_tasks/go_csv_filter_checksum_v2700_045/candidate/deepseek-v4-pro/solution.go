package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var b, c int
		hot := false
		fidx := 0
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				switch fidx {
				case 2:
					b = fastAtoi(row[start:i])
				case 3:
					c = fastAtoi(row[start:i])
				case 4:
					s := row[start:i]
					hot = len(s) == 3 && s[0] == 'h' && s[1] == 'o' && s[2] == 't'
				}
				fidx++
				start = i + 1
				if fidx > 4 {
					break
				}
			}
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
	if len(s) == 0 {
		return 0
	}
	i := 0
	neg := false
	if s[0] == '-' {
		neg = true
		i = 1
	} else if s[0] == '+' {
		i = 1
	}
	n := 0
	for ; i < len(s); i++ {
		c := s[i]
		if c < '0' || c > '9' {
			return 0
		}
		n = n*10 + int(c-'0')
	}
	if neg {
		return -n
	}
	return n
}
