package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var commas [5]int
		n := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commas[n] = i
				n++
				if n >= 5 {
					break
				}
			}
		}
		if n < 4 {
			continue
		}

		b := parseUint(row, commas[1]+1, commas[2])
		c := parseUint(row, commas[2]+1, commas[3])

		f4start := commas[3] + 1
		f4end := commas[4]
		if n < 5 {
			f4end = len(row)
		}
		hot := f4end-f4start == 3 && row[f4start] == 'h' && row[f4start+1] == 'o' && row[f4start+2] == 't'

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseUint(s string, start, end int) uint64 {
	var n uint64
	for i := start; i < end; i++ {
		n = n*10 + uint64(s[i] - '0')
	}
	return n
}
