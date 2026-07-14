package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			// Early exit checks ordered by likelihood of failure
			if row[6] != '-') {
				goto endCheck
			}
			if row[0] < 'A' || row[0] > 'Z' || row[1] < 'A' || row[1] > 'Z' {
				goto endCheck
			}
			if row[2] < '0' || row[2] > '9' || row[3] < '0' || row[3] > '9' || row[4] < '0' || row[4] > '9' || row[5] < '0' || row[5] > '9' {
				goto endCheck
			}
			for i := 7; i <= 14; i++ {
				c := row[i]
				if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
					goto endCheck
				}
			}
			// All validation passed, compute value same as original implementation
			v = uint64(row[2] + row[7])
		endCheck:
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}