package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	const prime uint64 = 1099511628211
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i++ {
			id := ids[i]
			hash = (hash ^ uint64(id&0xFF)) * prime
			hash = (hash ^ uint64((id>>8)&0xFF)) * prime
			hash = (hash ^ uint64((id>>16)&0xFF)) * prime
			hash = (hash ^ uint64(id>>24)) * prime

			t := ts[i]
			hash = (hash ^ uint64(t&0xFF)) * prime
			hash = (hash ^ uint64((t>>8)&0xFF)) * prime
			hash = (hash ^ uint64((t>>16)&0xFF)) * prime
			hash = (hash ^ uint64(t>>24)) * prime

			lv := levels[i]
			hash = (hash ^ uint64(lv&0xFF)) * prime
			hash = (hash ^ uint64(lv>>8)) * prime

			ln := lens[i]
			hash = (hash ^ uint64(ln&0xFF)) * prime
			hash = (hash ^ uint64(ln>>8)) * prime
		}
	}
	return hash
}