package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4
		// Field indices: 0, 1, 2, 3, 4
		// We need to skip 2 commas to reach field 2
		i := 0
		n := len(row)

		// Skip field 0
		for i < n && row[i] != ',' {
			i++
		}
		i++ // skip comma

		// Skip field 1
		for i < n && row[i] != ',' {
			i++
		}
		i++ // skip comma

		// Parse field 2 (b)
		start := i
		for i < n && row[i] != ',' {
			i++
		}
		b := parseIntFast(row[start:i])
		i++ // skip comma

		// Parse field 3 (c)
		start = i
		for i < n && row[i] != ',' {
			i++
		}
		c := parseIntFast(row[start:i])
		i++ // skip comma

		// Parse field 4 (hot check)
		start = i
		for i < n && row[i] != ',' {
			i++
		}
		// Check if field 4 == "hot"
		hot := (i-start == 3) && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseIntFast(s string) int {
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
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}
