package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const seed = uint64(1469598103934665603)
	const mult = uint64(1099511628211)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = seed
		for i := range ids {
			// id bytes (little-endian)
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= mult
			hash ^= uint64(byte(v >> 8))
			hash *= mult
			hash ^= uint64(byte(v >> 16))
			hash *= mult
			hash ^= uint64(byte(v >> 24))
			hash *= mult

			// ts bytes
			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= mult
			hash ^= uint64(byte(v >> 8))
			hash *= mult
			hash ^= uint64(byte(v >> 16))
			hash *= mult
			hash ^= uint64(byte(v >> 24))
			hash *= mult

			// levels (uint16)
			w := levels[i]
			hash ^= uint64(byte(w))
			hash *= mult
			hash ^= uint64(byte(w >> 8))
			hash *= mult

			// lens (uint16)
			w = lens[i]
			hash ^= uint64(byte(w))
			hash *= mult
			hash ^= uint64(byte(w >> 8))
			hash *= mult
		}
	}
	return hash
}
