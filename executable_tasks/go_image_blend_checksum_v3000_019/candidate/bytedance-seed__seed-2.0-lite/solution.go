package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	out := make([]byte, len(src))
	var hash uint64
	pixels := width * height
	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := 255 - a
			r := uint8((uint32(src[base])*uint32(a) + uint32(dst[base])*uint32(invA) + 127) / 255)
			g := uint8((uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*uint32(invA) + 127) / 255)
			b := uint8((uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*uint32(invA) + 127) / 255)
			out[base], out[base+1], out[base+2], out[base+3] = r, g, b, 255
			hash ^= uint64(r)
			hash *= fnvPrime
			hash ^= uint64(g)
			hash *= fnvPrime
			hash ^= uint64(b)
			hash *= fnvPrime
			hash ^= uint64(255)
			hash *= fnvPrime
		}
	}
	return hash
}