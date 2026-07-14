package main

func run(src, dst []byte, width, height, iters int) uint64 {
	out := make([]byte, len(src))
	var hash uint64
	const fnvPrime = 1099511628211
	const fnvOffset = 1469598103934665603
	pixels := width * height

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]
			ua := uint32(a)
			uinvA := uint32(255 - a)

			r := uint32(src[base])*ua + uint32(dst[base])*uinvA + 127
			v := uint8(r / 255)
			out[base] = v
			hash ^= uint64(v)
			hash *= fnvPrime

			r = uint32(src[base+1])*ua + uint32(dst[base+1])*uinvA + 127
			v = uint8(r / 255)
			out[base+1] = v
			hash ^= uint64(v)
			hash *= fnvPrime

			r = uint32(src[base+2])*ua + uint32(dst[base+2])*uinvA + 127
			v = uint8(r / 255)
			out[base+2] = v
			hash ^= uint64(v)
			hash *= fnvPrime

			out[base+3] = 255
			hash ^= uint64(255)
			hash *= fnvPrime
		}
	}
	return hash
}