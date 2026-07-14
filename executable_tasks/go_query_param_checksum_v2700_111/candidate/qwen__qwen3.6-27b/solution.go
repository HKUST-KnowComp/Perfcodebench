package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var cur int
		var field int
		for i := 0; i < len(row); i++ {
			c := row[i]
			if c == '&' {
				if field == 0 {
					u = cur
				} else {
					score = cur
				}
				cur = 0
				field++
			} else if c >= '0' && c <= '9' {
				cur = cur*10 + int(c-'0')
			}
		}
		flag = cur
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}