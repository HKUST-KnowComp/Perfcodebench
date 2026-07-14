package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const (
		initialHash uint64 = 1469598103934665603
		prime       uint64 = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = initialHash
		n := len(ids)
		for i := 0; i < n; i++ {
			// id (uint32) little-endian
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			// ts (uint32) little-endian
			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			// levels (uint16) little-endian
			lv := levels[i]
			hash ^= uint64(byte(lv))
			hash *= prime
			hash ^= uint64(byte(lv >> 8))
			hash *= prime

			// lens (uint16) little-endian
			lv = lens[i]
			hash ^= uint64(byte(lv))
			hash *= prime
			hash ^= uint64(byte(lv >> 8))
			hash *= prime
		}
	}
	return hash
}
