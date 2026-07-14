package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	n := len(keys)
	b := int(buckets)
	hist := make([]uint64, b)
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	for iter := 0; iter < iters; iter++ {
		// Accumulate histogram
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		// Compute checksum and zero histogram in one pass
		h = fnvOffset
		for i := 0; i < b; i++ {
			v := hist[i]
			h ^= v + 1
			h *= fnvPrime
			hist[i] = 0
		}
	}
	return h
}
