package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			// Validate all constraints matching original regex exactly
			if row[0] >= 'A' && row[0] <= 'Z' &&
				row[1] >= 'A' && row[1] <= 'Z' &&
				row[2] >= '0' && row[2] <= '9' &&
				row[3] >= '0' && row[3] <= '9' &&
				row[4] >= '0' && row[4] <= '9' &&
				row[5] >= '0' && row[5] <= '9' &&
				row[6] == '-' &&
				((row[7] >= '0' && row[7] <= '9') || (row[7] >= 'A' && row[7] <= 'F')) &&
				((row[8] >= '0' && row[8] <= '9') || (row[8] >= 'A' && row[8] <= 'F')) &&
				((row[9] >= '0' && row[9] <= '9') || (row[9] >= 'A' && row[9] <= 'F')) &&
				((row[10] >= '0' && row[10] <= '9') || (row[10] >= 'A' && row[10] <= 'F')) &&
				((row[11] >= '0' && row[11] <= '9') || (row[11] >= 'A' && row[11] <= 'F')) &&
				((row[12] >= '0' && row[12] <= '9') || (row[12] >= 'A' && row[12] <= 'F')) &&
				((row[13] >= '0' && row[13] <= '9') || (row[13] >= 'A' && row[13] <= 'F')) &&
				((row[14] >= '0' && row[14] <= '9') || (row[14] >= 'A' && row[14] <= 'F')) {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}