package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}
		// Compute checksum and reset histogram in one pass
		h = 1469598103934665603
		for i, v := range hist {
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0 // zero for next iteration
		}
	}
	return h
}
