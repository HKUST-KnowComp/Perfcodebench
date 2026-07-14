package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var nums [3]int
		idx := 0
		val := 0
		neg := false
		parsing := false
		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
				parsing = true
			} else if c == '-' {
				neg = true
				parsing = true
			} else {
				if parsing {
					if neg {
						val = -val
					}
					if idx < 3 {
						nums[idx] = val
						idx++
					}
					val = 0
					neg = false
					parsing = false
				}
			}
		}
		if parsing && idx < 3 {
			if neg {
				val = -val
			}
			nums[idx] = val
		}

		v := uint64((nums[0] & 2047) + nums[1] + nums[2]*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
