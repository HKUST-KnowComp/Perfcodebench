package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var b, c int
		hot := false
		fieldIdx := 0
		start := 0
		rowLen := len(row)
		for i := 0; i <= rowLen; i++ {
			if i == rowLen || row[i] == ',' {
				switch fieldIdx {
				case 2:
					b = parseIntSubstring(row, start, i)
				case 3:
					c = parseIntSubstring(row, start, i)
				case 4:
					hot = (i-start == 3) && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
				}
				fieldIdx++
				start = i + 1
				if fieldIdx > 4 {
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

func parseIntSubstring(s string, start, end int) int {
	n := 0
	for i := start; i < end; i++ {
		n = n*10 + int(s[i]-'0')
	}
	return n
}
