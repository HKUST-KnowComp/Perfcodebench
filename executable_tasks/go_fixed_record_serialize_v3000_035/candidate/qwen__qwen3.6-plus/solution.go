package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const prime = uint64(1099511628211)
	const offset = uint64(1469598103934665603)
	n := len(ids)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = offset
		for i := 0; i < n; i++ {
			id := ids[i]
			hash ^= uint64(id & 0xff); hash *= prime
			hash ^= uint64((id >> 8) & 0xff); hash *= prime
			hash ^= uint64((id >> 16) & 0xff); hash *= prime
			hash ^= uint64(id >> 24); hash *= prime

			tsVal := ts[i]
			hash ^= uint64(tsVal & 0xff); hash *= prime
			hash ^= uint64((tsVal >> 8) & 0xff); hash *= prime
			hash ^= uint64((tsVal >> 16) & 0xff); hash *= prime
			hash ^= uint64(tsVal >> 24); hash *= prime

			lv := levels[i]
			hash ^= uint64(lv & 0xff); hash *= prime
			hash ^= uint64(lv >> 8); hash *= prime

			ln := lens[i]
			hash ^= uint64(ln & 0xff); hash *= prime
			hash ^= uint64(ln >> 8); hash *= prime
		}
	}
	return hash
}
