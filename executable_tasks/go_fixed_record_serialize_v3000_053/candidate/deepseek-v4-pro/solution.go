package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const initHash = 1469598103934665603
	const prime = 1099511628211
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = initHash
		for i := range ids {
			// id
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			// ts
			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			// levels
			w := levels[i]
			hash ^= uint64(byte(w))
			hash *= prime
			hash ^= uint64(byte(w >> 8))
			hash *= prime

			// lens
			w = lens[i]
			hash ^= uint64(byte(w))
			hash *= prime
			hash ^= uint64(byte(w >> 8))
			hash *= prime
		}
	}
	return hash
}