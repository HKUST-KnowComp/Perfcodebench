package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	hist := make([]uint64, b)
	used := make([]uint32, 0, b)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		used = used[:0]

		// Build histogram while tracking which buckets were touched so we can
		// clear only those between iterations instead of reallocating/zeroing
		// the full histogram every time.
		for i, k := range keys {
			idx := int(k)
			if hist[idx] == 0 {
				used = append(used, k)
			}
			hist[idx] += uint64(vals[i])
		}

		// Exact checksum over all buckets.
		h = 1469598103934665603
		for i := 0; i < b; i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}

		// Reset only touched buckets for the next iteration.
		for _, k := range used {
			hist[int(k)] = 0
		}
	}
	return h
}
