package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	var hash uint64
	const prime = uint64(1099511628211)
	const offset = uint64(1469598103934665603)

	for iter := 0; iter < iters; iter++ {
		hash = offset
		for i := 0; i < n; i++ {
			v := ids[i]
			hash = (hash ^ uint64(v&0xFF)) * prime
			hash = (hash ^ uint64((v>>8)&0xFF)) * prime
			hash = (hash ^ uint64((v>>16)&0xFF)) * prime
			hash = (hash ^ uint64((v>>24)&0xFF)) * prime

			v = ts[i]
			hash = (hash ^ uint64(v&0xFF)) * prime
			hash = (hash ^ uint64((v>>8)&0xFF)) * prime
			hash = (hash ^ uint64((v>>16)&0xFF)) * prime
			hash = (hash ^ uint64((v>>24)&0xFF)) * prime

			v = uint32(levels[i])
			hash = (hash ^ uint64(v&0xFF)) * prime
			hash = (hash ^ uint64((v>>8)&0xFF)) * prime

			v = uint32(lens[i])
			hash = (hash ^ uint64(v&0xFF)) * prime
			hash = (hash ^ uint64((v>>8)&0xFF)) * prime
		}
	}
	return hash
}