package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	var hash uint64
	fnvOffset := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i * 4
			// Load source and destination pixel components
			sR, sG, sB, a := src[base], src[base+1], src[base+2], src[base+3]
			dR, dG, dB := dst[base], dst[base+1], dst[base+2]
			invA := 255 - a

			// Blend red channel
			blend := uint32(sR)*uint32(a) + uint32(dR)*uint32(invA)
			blend += 127
			outR := uint8(blend / 255)

			// Blend green channel
			blend = uint32(sG)*uint32(a) + uint32(dG)*uint32(invA)
			blend += 127
			outG := uint8(blend / 255)

			// Blend blue channel
			blend = uint32(sB)*uint32(a) + uint32(dB)*uint32(invA)
			blend += 127
			outB := uint8(blend / 255)

			// Update FNV-1a hash in identical byte order to original implementation
			hash ^= uint64(outR)
			hash *= fnvPrime
			hash ^= uint64(outG)
			hash *= fnvPrime
			hash ^= uint64(outB)
			hash *= fnvPrime
			hash ^= uint64(255)
			hash *= fnvPrime
		}
	}
	return hash
}