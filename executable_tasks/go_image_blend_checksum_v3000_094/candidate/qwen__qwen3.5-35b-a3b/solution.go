package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	if pixels == 0 {
		return 0
	}
	out := make([]byte, len(src))
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211

	for iter := 0; iter < iters; iter++ {
		var hash uint64 = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := 255 - a

			// Blend R
			sR := uint32(src[base])
			dR := uint32(dst[base])
			valR := sR*uint32(a) + dR*uint32(invA) + 127
			out[base] = uint8((valR * 257) >> 16)

			// Blend G
			sG := uint32(src[base+1])
			dG := uint32(dst[base+1])
			valG := sG*uint32(a) + dG*uint32(invA) + 127
			out[base+1] = uint8((valG * 257) >> 16)

			// Blend B
			sB := uint32(src[base+2])
			dB := uint32(dst[base+2])
			valB := sB*uint32(a) + dB*uint32(invA) + 127
			out[base+2] = uint8((valB * 257) >> 16)

			// Alpha
			out[base+3] = 255

			// Update Hash Incrementally
			hash ^= uint64(out[base])
			hash *= fnvPrime
			hash ^= uint64(out[base+1])
			hash *= fnvPrime
			hash ^= uint64(out[base+2])
			hash *= fnvPrime
			hash ^= uint64(out[base+3])
			hash *= fnvPrime
		}
	}
	return hash
}