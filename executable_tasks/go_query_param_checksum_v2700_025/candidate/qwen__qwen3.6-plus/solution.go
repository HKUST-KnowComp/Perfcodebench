package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag uint64
		var cur uint64
		var idx int
		inNum := false
		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				cur = cur*10 + uint64(c-'0')
				inNum = true
			} else if inNum {
				switch idx {
				case 0: u = cur
				case 1: score = cur
				case 2: flag = cur
				}
				idx++
				cur = 0
				inNum = false
			}
		}
		if inNum {
			switch idx {
			case 0: u = cur
			case 1: score = cur
			case 2: flag = cur
			}
		}
		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}