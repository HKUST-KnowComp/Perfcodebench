package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var stage int
		
		// Manual parser to avoid allocations
		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				current = current*10 + int(c-'0')
			} else if c == '&' || i == len(row)-1 {
				if i == len(row)-1 && c >= '0' && c <= '9' {
					flag = current
				} else {
					switch stage {
					case 0:
						u = current
					case 1:
						score = current
					}
					stage++
					current = 0
				}
			}
		}
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}