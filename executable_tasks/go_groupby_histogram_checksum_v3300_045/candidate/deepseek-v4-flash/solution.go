package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		clear(hist)
		for i := 0; i < len(keys); i++ {
			k := keys[i]
			v := vals[i]
			hist[k] += uint64(v)
		}
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}