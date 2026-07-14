package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)

	for iter := 0; iter < iters; iter++ {
		// Aggregate values into histogram
		n := len(keys)
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// Compute checksum and zero histogram in one pass
		h = 1469598103934665603
		l := len(hist)
		for i := 0; i < l; i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0
		}
	}
	return h
}