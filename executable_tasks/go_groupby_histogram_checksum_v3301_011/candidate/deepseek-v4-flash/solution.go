package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Accumulate keys into histogram
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}
		if iter == iters-1 {
			// Last iteration: compute checksum only
			h = 1469598103934665603
			for _, v := range hist {
				h ^= v + 1
				h *= 1099511628211
			}
		} else {
			// Non-last iteration: compute checksum and zero histogram in one loop
			h = 1469598103934665603
			for i, v := range hist {
				h ^= v + 1
				h *= 1099511628211
				hist[i] = 0
			}
		}
	}
	return h
}
