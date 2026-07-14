package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	const fnvInit = 1469598103934665603
	const fnvPrime = 1099511628211
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = fnvInit
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := uint32(src[base+3])
			invA := 255 - a

			r := (uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255
			hash ^= uint64(r)
			hash *= fnvPrime

			g := (uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255
			hash ^= uint64(g)
			hash *= fnvPrime

			b := (uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255
			hash ^= uint64(b)
			hash *= fnvPrime

			hash ^= 255
			hash *= fnvPrime
		}
	}
	return hash
}
