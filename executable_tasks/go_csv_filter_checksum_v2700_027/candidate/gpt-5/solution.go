package main

func atoiField(s string, start, end int) int {
	if start >= end {
		return 0
	}
	i := start
	neg := false
	c := s[i]
	if c == '-' {
		neg = true
		i++
		if i >= end {
			return 0
		}
	} else if c == '+' {
		i++
		if i >= end {
			return 0
		}
	}
	val := 0
	for ; i < end; i++ {
		ch := s[i]
		if ch < '0' || ch > '9' {
			return 0
		}
		val = val*10 + int(ch-'0')
	}
	if neg {
		val = -val
	}
	return val
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		s := row
		n := len(s)
		field := 0
		start := 0
		b := 0
		c := 0
		hot := false
		for i := 0; i <= n; i++ {
			if i == n || s[i] == ',' {
				if field == 2 {
					b = atoiField(s, start, i)
				} else if field == 3 {
					c = atoiField(s, start, i)
				} else if field == 4 {
					segLen := i - start
					if segLen == 3 && s[start] == 'h' && s[start+1] == 'o' && s[start+2] == 't' {
						hot = true
					} else {
						hot = false
					}
					// We have all needed fields; stop scanning
					break
				}
				field++
				start = i + 1
			}
		}
		sum := b + c + 11
		if hot {
			sum += 29
		}
		v := uint64(sum)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
