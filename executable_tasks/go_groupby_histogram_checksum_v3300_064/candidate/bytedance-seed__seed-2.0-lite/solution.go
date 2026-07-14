package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Preallocate histogram once to avoid per-iteration heap allocations
	hist := make([]uint64, buckets)
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211

	for iter := 0; iter < iters; iter++ {
		// Fast zero-reset of existing memory (optimized compiler intrinsic)
		clear(hist)
		// Maintain identical accumulation logic to preserve correctness
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact same checksum calculation as baseline to guarantee matching output
		h = fnvOffset
		for _, v := range hist {
			h ^= v + 1
			h *= fnvPrime
		}
	}
	return h
}