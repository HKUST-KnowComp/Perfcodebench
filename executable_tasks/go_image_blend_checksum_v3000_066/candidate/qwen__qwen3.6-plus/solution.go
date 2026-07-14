package main

func run(src, dst []byte, width, height, iters int) uint64 {
	const fnvInit = 1469598103934665603
	const fnvPrime = 1099511628211
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = fnvInit
		for i := 0; i < len(src); i += 4 {
			a := src[i+3]
			invA := 255 - a
			ua := uint32(a)
			uinvA := uint32(invA)

			r := (uint32(src[i])*ua + uint32(dst[i])*uinvA + 127) / 255
			hash ^= uint64(r)
			hash *= fnvPrime

			g := (uint32(src[i+1])*ua + uint32(dst[i+1])*uinvA + 127) / 255
			hash ^= uint64(g)
			hash *= fnvPrime

			b := (uint32(src[i+2])*ua + uint32(dst[i+2])*uinvA + 127) / 255
			hash ^= uint64(b)
			hash *= fnvPrime

			hash ^= 255
			hash *= fnvPrime
		}
	}
	return hash
}