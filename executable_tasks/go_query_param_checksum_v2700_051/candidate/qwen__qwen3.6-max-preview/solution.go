package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var nums [3]int
		idx := 0
		for i := 0; i < len(row) && idx < 3; i++ {
			if row[i] == '=' {
				i++
				val := 0
				neg := false
				if i < len(row) && row[i] == '-' {
					neg = true
					i++
				}
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					val = val*10 + int(row[i]-'0')
					i++
				}
				if neg {
					val = -val
				}
				nums[idx] = val
				idx++
			}
		}
		v := uint64((nums[0] & 2047) + nums[1] + nums[2]*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
