package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := uint64(1469598103934665603)
		for i := 0; i < n; i++ {
			id := ids[i]
			// id bytes (little-endian)
			b := byte(id)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(id >> 8)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(id >> 16)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(id >> 24)
			h ^= uint64(b)
			h *= 1099511628211

			tsv := ts[i]
			b = byte(tsv)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(tsv >> 8)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(tsv >> 16)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(tsv >> 24)
			h ^= uint64(b)
			h *= 1099511628211

			lev := levels[i]
			b = byte(lev)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(lev >> 8)
			h ^= uint64(b)
			h *= 1099511628211

			ln := lens[i]
			b = byte(ln)
			h ^= uint64(b)
			h *= 1099511628211
			b = byte(ln >> 8)
			h ^= uint64(b)
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}
