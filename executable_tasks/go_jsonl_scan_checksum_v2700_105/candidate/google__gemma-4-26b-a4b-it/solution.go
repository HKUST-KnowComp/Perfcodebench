package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We scan the row manually to avoid strings.Split allocations
		// and multiple strings.Contains passes.
		for i := 0; i < len(row); i++ {
			if row[i] == '"' {
				// Check for "user":
				if i+7 < len(row) && row[i+1:i+7] == "user":" {
					i += 7
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
					// If the format is "user":123 (no quotes around value), 
					// the logic below handles both cases by checking digits.
				} else if i+7 < len(row) && row[i+1:i+7] == "user":" {
					i += 7
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				} else if i+7 < len(row) && row[i+1:i+7] == "user": {
					i += 7
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				} else if i+8 < len(row) && row[i+1:i+8] == "score":" {
					i += 8
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				} else if i+7 < len(row) && row[i+1:i+7] == "score": {
					i += 7
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				} else if i+10 < len(row) && row[i+1:i+11] == "flag":true" {
					flag = true
				} else if i+10 < len(row) && row[i+1:i+11] == "flag":true {
					flag = true
				}
			}
		
		// Re-implementing the exact logic from baseline to ensure correctness
		// The baseline used strings.Contains which is slightly different from exact key matching
		// but since we are optimizing the same logic, we must ensure the same result.
		// Let's use a more robust single-pass approach that mimics the baseline's behavior.
		user = 0
		score = 0
		flag = false
		for j := 0; j < len(row); j++ {
			if j+7 <= len(row) && row[j:j+7] == ""user":" {
				for k := j + 7; k < len(row); k++ {
					if row[k] >= '0' && row[k] <= '9' {
						user = user*10 + int(row[k]-'0')
						} else if row[k] == ',' || row[k] == '}' || row[k] == '"' {
							break
						}
					}
				} else if j+8 <= len(row) && row[j:j+8] == ""score":" {
					for k := j + 8; k < len(row); k++ {
						if row[k] >= '0' && row[k] <= '9' {
							score = score*10 + int(row[k]-'0')
						} else if row[k] == ',' || row[k] == '}' || row[k] == '"' {
							break
						}
					}
				} else if j+10 <= len(row) && row[j:j+10] == ""flag":true" {
					flag = true
				}
			}
		
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// Corrected implementation to match baseline behavior exactly
func run_optimized(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// The baseline splits by comma first.
		// We can simulate this by scanning for commas.
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				// This is the 'part'
				part := row[start:i]
				
				// Check "user":
				if contains(part, "\"user\":") {
					for k := 0; k < len(part); k++ {
						if part[k] >= '0' && part[k] <= '9' {
							user = user*10 + int(part[k]-'0')
						}
					}
					} else if contains(part, "\"score\":") {
					for k := 0; k < len(part); k++ {
						if part[k] >= '0' && part[k] <= '9' {
							score = score*10 + int(part[k]-'0')
						}
					}
					} else if contains(part, "\"flag\":true") {
					flag = true
				}
				start = i + 1
			}
		}
		
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func contains(s, substr string) bool {
	if len(substr) == 0 { return true }
	if len(s) < len(substr) { return false }
	for i := 0; i <= len(s)-len(substr); i++ {
		if s[i:i+len(substr)] == substr {
			return true
		}
	}
	return false
}

// Final clean version for the actual return
func run_final(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				// part is row[start:i]
				// Instead of slicing, we check bounds manually to avoid allocations
					
				// Check "user":
				foundUser := false
				for j := start; j < i-6; j++ {
						if row[j:j+7] == "\"user\":" {
							foundUser = true
							break
						}
					}
					if foundUser {
						for k := start; k < i; k++ {
							if row[k] >= '0' && row[k] <= '9' {
								user = user*10 + int(row[k]-'0')
							}
						}
					} else {
					// Check "score":
					foundScore := false
					for j := start; j < i-7; j++ {
							if row[j:j+8] == "\"score":" {
								foundScore = true
								break
							}
						}
						if foundScore {
							for k := start; k < i; k++ {
								if row[k] >= '0' && row[k] <= '9' {
									score = score*10 + int(row[k]-'0')
								}
							}
						} else {
						// Check "flag":true
						foundFlag := false
						for j := start; j <= i-10; j++ {
							if row[j:j+10] == "\"flag\":true" {
								foundFlag = true
								break
							}
							}
							if foundFlag {
								flag = true
							}
						}
					}
					start = i + 1
				}
			}
			
			v := uint64((user & 1023) + score)
			if flag {
				v += 17
			}
			hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
