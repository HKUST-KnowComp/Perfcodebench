package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := parseValue(row, 2) // skip 'u=' (2 chars)
		
		// Find start of score=
		idx := 0
		for idx < len(row) && row[idx] != 's' { idx++ }
		score := parseValue(row, idx+6) // skip 'score=' (6 chars)
		
		// Find start of flag=
		for idx < len(row) && row[idx] != 'f' { idx++ }
		flag := parseValue(row, idx+5) // skip 'flag=' (5 chars)
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseValue(s string, start int) int {
	if start >= len(s) {
		return 0
	}
	res := 0
	for i := start; i < len(s); i++ {
		c := s[i]
		if c == '&' {
			break
		}
		if c < '0' || c > '9' {
			break
		}
		res = res*10 + int(c-'0')
	}
	return res
}