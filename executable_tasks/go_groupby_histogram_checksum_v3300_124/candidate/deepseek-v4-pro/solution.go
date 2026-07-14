package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	_ = hist[buckets-1] // inform compiler that indices < buckets are safe
	for iter := 0; iter < iters; iter++ {
		// Aggregate into histogram
		for i, key := range keys {
			hist[key] += uint64(vals[i])
		}
		// Compute checksum from histogram
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
		// Reset histogram for next iteration, except the last
		if iter < iters-1 {
			for i := range hist {
				hist[i] = 0
			}
		}
	}
	return h
}
