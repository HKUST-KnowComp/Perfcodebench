package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	var touched []uint32
	if len(keys) < int(buckets)/4 {
		touched = make([]uint32, 0, len(keys))
	}
	
	for iter := 0; iter < iters; iter++ {
		if touched != nil {
			for i := range keys {
				k := keys[i]
				if hist[k] == 0 {
					touched = append(touched, k)
				}
				hist[k] += uint64(vals[i])
			}
		} else {
			for i := range keys {
				hist[keys[i]] += uint64(vals[i])
			}
		}
		
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
		
		if touched != nil {
			for _, k := range touched {
				hist[k] = 0
			}
			touched = touched[:0]
		} else {
			for i := range hist {
				hist[i] = 0
			}
		}
	}
	return h
}