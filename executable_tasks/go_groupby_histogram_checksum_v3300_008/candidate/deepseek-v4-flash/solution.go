package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Aggregate keys into histogram
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}
		// Compute checksum and clear histogram in one pass
		h = 1469598103934665603
		for i, v := range hist {
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0
		}
	}
	return h
}
