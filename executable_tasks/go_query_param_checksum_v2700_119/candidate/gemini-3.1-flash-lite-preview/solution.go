package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var val int
		var state int // 0: u, 1: score, 2: flag
		
		for i := 0; i < len(row); i++ {
			if row[i] >= '0' && row[i] <= '9' {
				val = val*10 + int(row[i]-'0')
			} else if row[i] == '=' {
				val = 0
			} else if row[i] == '&' || i == len(row)-1 {
				switch state {
				case 0:
					u = val
				case 1:
					score = val
				case 2:
					flag = val
				}
				state++
				val = 0
			}
		}
		
		hash ^= uint64((u&2047)+score+flag*19) + 1
		hash *= 1099511628211
	}
	return hash
}