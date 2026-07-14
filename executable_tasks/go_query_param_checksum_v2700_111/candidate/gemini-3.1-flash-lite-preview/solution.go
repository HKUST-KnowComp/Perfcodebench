package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var val int
		var stage int
		for i := 0; i < len(row); i++ {
			if row[i] >= '0' && row[i] <= '9' {
				val = val*10 + int(row[i]-'0')
			} else if row[i] == '&' || i == len(row)-1 {
				if i == len(row)-1 && row[i] >= '0' && row[i] <= '9' {
					val = val*10 + int(row[i]-'0')
				}
				switch stage {
				case 0:
					u = val
				case 1:
					score = val
				case 2:
					flag = val
				}
				val = 0
				stage++
			} else if row[i] == '=' {
				val = 0
			}
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}