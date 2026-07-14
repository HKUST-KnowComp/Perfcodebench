package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	
	for iter := 0; iter < iters; iter++ {
		// Zero out histogram
		for i := range hist {
			hist[i] = 0
		}
		
		// Accumulate with loop unrolling
		n := len(keys)
		i := 0
		for i+3 < n {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
			i += 4
		}
		for ; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		
		// Compute checksum with loop unrolling
		h = 1469598103934665603
		const fnvPrime = 1099511628211
		i = 0
		for i+3 < int(buckets) {
			h ^= hist[i] + 1
			h *= fnvPrime
			h ^= hist[i+1] + 1
			h *= fnvPrime
			h ^= hist[i+2] + 1
			h *= fnvPrime
			h ^= hist[i+3] + 1
			h *= fnvPrime
			i += 4
		}
		for ; i < int(buckets); i++ {
			h ^= hist[i] + 1
			h *= fnvPrime
		}
	}
	return h
}