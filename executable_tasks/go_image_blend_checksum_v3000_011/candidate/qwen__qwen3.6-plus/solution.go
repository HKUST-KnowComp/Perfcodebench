package main

func run(src, dst []byte, width, height, iters int) uint64 {
	const fnvPrime = 1099511628211
	const fnvOffset = 1469598103934665603
	pixels := width * height
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := 255 - a
			
			ua := uint32(a)
			uinvA := uint32(invA)
			
			r := (uint32(src[base])*ua + uint32(dst[base])*uinvA + 127) / 255
			hash ^= uint64(r)
			hash *= fnvPrime
			
			g := (uint32(src[base+1])*ua + uint32(dst[base+1])*uinvA + 127) / 255
			hash ^= uint64(g)
			hash *= fnvPrime
			
			b := (uint32(src[base+2])*ua + uint32(dst[base+2])*uinvA + 127) / 255
			hash ^= uint64(b)
			hash *= fnvPrime
			
			hash ^= 0xFF
			hash *= fnvPrime
		}
	}
	return hash
}