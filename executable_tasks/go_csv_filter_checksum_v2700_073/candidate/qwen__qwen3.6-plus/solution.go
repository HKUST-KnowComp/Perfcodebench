package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		commaCount := 0
		fieldStart := 0
		b, c := 0, 0
		hot := false
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					fieldStart = i + 1
				} else if commaCount == 3 {
					b = parseIntFast(row, fieldStart, i)
					fieldStart = i + 1
				} else if commaCount == 4 {
					c = parseIntFast(row, fieldStart, i)
					fieldStart = i + 1
					rem := row[fieldStart:]
					hot = len(rem) >= 3 && rem[0] == 'h' && rem[1] == 'o' && rem[2] == 't' && (len(rem) == 3 || rem[3] == ',')
					break
				}
			}
		}
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseIntFast(s string, start, end int) int {
	val := 0
	neg := false
	for i := start; i < end; i++ {
		c := s[i]
		if c == '-' {
			neg = true
		} else if c == '+' {
			continue
		} else {
			val = val*10 + int(c-'0')
		}
	}
	if neg {
		return -val
	}
	return val
}
