package main

const (
	fnvOffset64 = 1469598103934665603
	fnvPrime64  = 1099511628211
)

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = fnvOffset64
	for _, b := range data {
		hash ^= uint64(b)
		hash *= fnvPrime64
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	var hash uint64
	p := uint64(fnvPrime64)
	offset := uint64(fnvOffset64)
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < n; i++ {
			v := ids[i]
			h ^= uint64(v & 0xFF)
			h *= p
			h ^= uint64((v >> 8) & 0xFF)
			h *= p
			h ^= uint64((v >> 16) & 0xFF)
			h *= p
			h ^= uint64((v >> 24) & 0xFF)
			h *= p

			v = ts[i]
			h ^= uint64(v & 0xFF)
			h *= p
			h ^= uint64((v >> 8) & 0xFF)
			h *= p
			h ^= uint64((v >> 16) & 0xFF)
			h *= p
			h ^= uint64((v >> 24) & 0xFF)
			h *= p

			v2 := levels[i]
			h ^= uint64(v2 & 0xFF)
			h *= p
			h ^= uint64((v2 >> 8) & 0xFF)
			h *= p

			v2 = lens[i]
			h ^= uint64(v2 & 0xFF)
			h *= p
			h ^= uint64((v2 >> 8) & 0xFF)
			h *= p
		}
		hash = h
	}
	return hash
}
