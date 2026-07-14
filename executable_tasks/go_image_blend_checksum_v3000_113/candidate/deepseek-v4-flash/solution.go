package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen) // preallocate once

	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603 // FNV offset basis

		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := 255 - a

			// blend each channel directly into out
			b0 := uint32(src[base])*uint32(a) + uint32(dst[base])*uint32(invA) + 127
			b1 := uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*uint32(invA) + 127
			b2 := uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*uint32(invA) + 127

			out[base] = byte(b0 / 255)
			out[base+1] = byte(b1 / 255)
			out[base+2] = byte(b2 / 255)
			out[base+3] = 255

			// update hash as we write each byte
			hash ^= uint64(out[base])
			hash *= 1099511628211
			hash ^= uint64(out[base+1])
			hash *= 1099511628211
			hash ^= uint64(out[base+2])
			hash *= 1099511628211
			hash ^= uint64(out[base+3])
			hash *= 1099511628211
		}
	}
	return hash
}
