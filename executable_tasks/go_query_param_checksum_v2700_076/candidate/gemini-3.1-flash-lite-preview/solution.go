package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var val int
		var stage int
		
		// Manual parser to avoid allocations and overhead
		for i := 0; i < len(row); i++ {
			if row[i] >= '0' && row[i] <= '9' {
				val = val*10 + int(row[i]-'0')
			} else if row[i] == '&' {
				switch stage {
				case 0: u = val
				case 1: score = val
				}
				val = 0
				stage++
			}
		}
		flag = val

		v := uint64((u & 2047) + score + flag*19)
		hash ^= (v + 1)
		hash *= 1099511628211
	}
	return hash
}