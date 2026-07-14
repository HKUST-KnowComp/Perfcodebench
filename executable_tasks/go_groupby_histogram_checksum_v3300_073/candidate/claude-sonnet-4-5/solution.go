package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	
	for iter := 0; iter < iters; iter++ {
		// Zero out histogram
		for i := range hist {
			hist[i] = 0
		}
		
		// Accumulate with unrolling
		i := 0
		for i+3 < len(keys) {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
			i += 4
		}
		for ; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		
		// Compute checksum with unrolling
		h = 1469598103934665603
		i = 0
		for i+3 < len(hist) {
			h ^= hist[i] + 1
			h *= 1099511628211
			h ^= hist[i+1] + 1
			h *= 1099511628211
			h ^= hist[i+2] + 1
			h *= 1099511628211
			h ^= hist[i+3] + 1
			h *= 1099511628211
			i += 4
		}
		for ; i < len(hist); i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}
	return h
}