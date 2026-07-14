package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Pre-allocate histogram and fill with 1 to account for the +1 in the checksum loop
	hist := make([]uint64, buckets)
	for i := range hist {
		hist[i] = 1
	}
	// Aggregate values (performed once)
	for i := 0; i < len(keys); i++ {
		hist[keys[i]] += uint64(vals[i])
	}
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603
		for i := 0; i < len(hist); i++ {
			v := hist[i]
			h ^= v
			h *= 1099511628211
		}
	}
	return h
}
