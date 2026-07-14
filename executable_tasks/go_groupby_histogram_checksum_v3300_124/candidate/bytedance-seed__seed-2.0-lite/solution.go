package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Allocate histogram once to avoid repeated heap allocations
	hist := make([]uint64, buckets)
	inputLen := len(keys)
	for iter := 0; iter < iters; iter++ {
		// Populate histogram with identical logic to original implementation
		for i := 0; i < inputLen; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		// Reset FNV-like hash state to original baseline value
		h = 1469598103934665603
		// Compute checksum while zeroing histogram to reuse for next iteration
		for i := range hist {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0
		}
	}
	return h
}