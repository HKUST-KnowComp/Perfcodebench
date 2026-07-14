package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	for iter := 0; iter < iters; iter++ {
		hist := make([]uint64, buckets)
		n := len(keys)
		for i := 0; i < n; {
			// unroll 4x to hide latency and eliminate bounds checks
			k0, k1, k2, k3 := keys[i], keys[i+1], keys[i+2], keys[i+3]
			v0, v1, v2, v3 := vals[i], vals[i+1], vals[i+2], vals[i+3]
			hist[k0] += uint64(v0)
			hist[k1] += uint64(v1)
			hist[k2] += uint64(v2)
			hist[k3] += uint64(v3)
			i += 4
		}
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}