package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	n := len(keys)

	for iter := 0; iter < iters; iter++ {
		for i := range hist {
			hist[i] = 0
		}

		i := 0
		for ; i+3 < n; i += 4 {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
		}
		for ; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}