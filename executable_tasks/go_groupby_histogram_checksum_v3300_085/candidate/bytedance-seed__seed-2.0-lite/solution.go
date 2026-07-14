package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Preallocate histogram once outside iteration loop to avoid repeated heap allocation overhead
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Fast zeroing of histogram using built-in clear intrinsic, replaces expensive new slice allocation
		clear(hist)
		// Aggregate values identically to original to preserve correct bucket sums
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact same checksum calculation as original to guarantee matching output
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}