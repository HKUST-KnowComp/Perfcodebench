package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var vals [3]int
		var val int
		var field int
		var neg bool
		var inNum bool

		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
				inNum = true
			} else if c == '-' {
				neg = true
			} else if inNum {
				if neg {
					val = -val
				}
				if field < 3 {
					vals[field] = val
				}
				field++
				val = 0
				neg = false
				inNum = false
			}
		}
		if inNum {
			if neg {
				val = -val
			}
			if field < 3 {
				vals[field] = val
			}
		}

		v := uint64((vals[0] & 2047) + vals[1] + vals[2]*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}