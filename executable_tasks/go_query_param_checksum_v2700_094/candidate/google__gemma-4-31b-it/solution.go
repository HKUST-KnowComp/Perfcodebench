package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		pos := 0
		
		// Parse u=
		for pos < len(row) && row[pos] != '=' {
			pos++
		}
		pos++ // skip '='
		for pos < len(row) && row[pos] != '&' {
			u = u*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip '&'
		
		// Parse score=
		for pos < len(row) && row[pos] != '=' {
			pos++
		}
		pos++ // skip '='
		for pos < len(row) && row[pos] != '&' {
			score = score*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip '&'
		
		// Parse flag=
		for pos < len(row) && row[pos] != '=' {
			pos++
		}
		pos++ // skip '='
		for pos < len(row) {
			flag = flag*10 + int(row[pos]-'0')
			pos++
		}
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}