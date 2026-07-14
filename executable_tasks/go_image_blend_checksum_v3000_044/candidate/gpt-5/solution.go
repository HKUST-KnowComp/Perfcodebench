package main

func run(src, dst []byte, width, height, iters int) uint64 {
	// Streaming FNV-1a over blended output to avoid allocating/writing an output buffer.
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	end := width * height * 4
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for base := 0; base < end; base += 4 {
			// Alpha and its inverse for this pixel
			a := uint32(src[base+3])
			inv := uint32(255) - a

			// R channel
			y := uint32(src[base])*a + uint32(dst[base])*inv
			br := byte((y + 127) / 255)
			h ^= uint64(br)
			h *= fnvPrime

			// G channel
			y = uint32(src[base+1])*a + uint32(dst[base+1])*inv
			bg := byte((y + 127) / 255)
			h ^= uint64(bg)
			h *= fnvPrime

			// B channel
			y = uint32(src[base+2])*a + uint32(dst[base+2])*inv
			bb := byte((y + 127) / 255)
			h ^= uint64(bb)
			h *= fnvPrime

			// A channel is set to 255
			h ^= uint64(255)
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
