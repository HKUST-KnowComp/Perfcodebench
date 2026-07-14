package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// one reusable histogram buffer
	hist := make([]uint64, buckets)
	var h uint64 = 14695981039346656037

	for iter := 0; iter < iters; iter++ {
		// fast zero: single forward pass, no function calls
		for i := range hist {
			hist[i] = 0
		}

		// unroll aggregation 4×
		n := len(keys)
		for i := 0; i+3 < n; i += 4 {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
		}
		// tail
		for ; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// unroll checksum 4×
		const prime = 1099511628211
		m := int(buckets)
		j := 0
		for ; j+3 < m; j += 4 {
			h ^= hist[j] + 1
			h *= prime
			h ^= hist[j+1] + 1
			h *= prime
			h ^= hist[j+2] + 1
			h *= prime
			h ^= hist[j+3] + 1
			h *= prime
		}
		for ; j < m; j++ {
			h ^= hist[j] + 1
			h *= prime
		}
	}
	return h
}