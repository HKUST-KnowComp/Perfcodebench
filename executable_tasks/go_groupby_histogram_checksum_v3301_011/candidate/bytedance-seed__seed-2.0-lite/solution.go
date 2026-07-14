package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	if iters <= 0 {
		return h
	}
	// Allocate histogram once to avoid per-iteration heap allocation overhead
	hist := make([]uint64, buckets)
	const fnvInit = 1469598103934665603
	const fnvPrime = 1099511628211
	for iter := 0; iter < iters; iter++ {
		// Efficiently reset histogram to zero with runtime-optimized memclr via clear()
		clear(hist)
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		h = fnvInit
		for _, v := range hist {
			h ^= v + 1
			h *= fnvPrime
		}
	}
	return h
}