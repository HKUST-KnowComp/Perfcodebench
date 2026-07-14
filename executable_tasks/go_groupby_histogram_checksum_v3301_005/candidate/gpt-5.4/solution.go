package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	hist := make([]uint64, b)
	used := make([]uint32, 0, 256)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		for i, k := range keys {
			ki := int(k)
			if hist[ki] == 0 {
				used = append(used, k)
			}
			hist[ki] += uint64(vals[i])
		}

		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}

		for _, k := range used {
			hist[int(k)] = 0
		}
		used = used[:0]
	}
	return h
}
