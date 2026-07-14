package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Preallocate histogram once to avoid repeated heap allocation costs
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Efficiently zero buckets with assembly-optimized built-in clear()
		clear(hist)
		// Maintain original accumulation logic to preserve correctness
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact same checksum calculation as baseline to guarantee matching output
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}