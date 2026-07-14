package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	limit := pixels * 4
	src = src[:limit]
	dst = dst[:limit]

	var hash uint64
	const prime = 1099511628211
	const offset = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		hash = offset
		for base := 0; base < limit; base += 4 {
			a := src[base+3]
			invA := 255 - a
			ua := uint32(a)
			uinvA := uint32(invA)

			// R channel
			val := uint32(src[base])*ua + uint32(dst[base])*uinvA + 127
			hash ^= uint64(val / 255)
			hash *= prime

			// G channel
			val = uint32(src[base+1])*ua + uint32(dst[base+1])*uinvA + 127
			hash ^= uint64(val / 255)
			hash *= prime

			// B channel
			val = uint32(src[base+2])*ua + uint32(dst[base+2])*uinvA + 127
			hash ^= uint64(val / 255)
			hash *= prime

			// A channel (always 255)
			hash ^= 0xFF
			hash *= prime
		}
	}
	return hash
}
