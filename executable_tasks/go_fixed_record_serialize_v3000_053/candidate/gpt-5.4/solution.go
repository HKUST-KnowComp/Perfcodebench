package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	const recSize = 12
	total := n * recSize

	out := make([]byte, total)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		p := 0
		for i := 0; i < n; i++ {
			v0 := ids[i]
			out[p+0] = byte(v0)
			out[p+1] = byte(v0 >> 8)
			out[p+2] = byte(v0 >> 16)
			out[p+3] = byte(v0 >> 24)

			v1 := ts[i]
			out[p+4] = byte(v1)
			out[p+5] = byte(v1 >> 8)
			out[p+6] = byte(v1 >> 16)
			out[p+7] = byte(v1 >> 24)

			v2 := levels[i]
			out[p+8] = byte(v2)
			out[p+9] = byte(v2 >> 8)

			v3 := lens[i]
			out[p+10] = byte(v3)
			out[p+11] = byte(v3 >> 8)

			p += recSize
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
