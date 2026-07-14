package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	hash := offset
	for _, row := range rows {
		var b, c int
		var hot bool

		field := 0
		start := 0
		n := len(row)
		for i := 0; i <= n; i++ {
			if i == n || row[i] == ',' {
				// process field [start,i)
				switch field {
				case 2:
					// parse int for b
					b = fastAtoiSub(row, start, i)
				case 3:
					// parse int for c
					c = fastAtoiSub(row, start, i)
				case 4:
					// check if equals "hot"
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
				}
				field++
				if field > 4 {
					break
				}
				start = i + 1
			}
		}

		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

// fastAtoiSub parses an integer from s[start:end].
// It returns 0 on any invalid format (matching strconv.Atoi behavior when error is ignored).
func fastAtoiSub(s string, start, end int) int {
	if start >= end {
		return 0
	}
	i := start
	sign := 1
	c := s[i]
	if c == '+' {
		i++
	} else if c == '-' {
		sign = -1
		i++
	}
	if i >= end {
		return 0
	}
	val := 0
	for ; i < end; i++ {
		ch := s[i]
		if ch < '0' || ch > '9' {
			return 0
		}
		val = val*10 + int(ch-'0')
	}
	return sign * val
}
