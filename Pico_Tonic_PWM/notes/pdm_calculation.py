#!/usr/bin/env python3

def generate_pdm_word_16bit(value):
    """
    Generate a 32-bit PDM word from a 16-bit input value (0 to 65535).
    Uses first-order sigma-delta modulation.
    """
    global accumulator
    bits = 0
    
    for _ in range(32):
        if accumulator >= 0:
            target = 65535
        else:
            target = 0
            
        error = value - target
        print(error)
        accumulator += error
        
        if accumulator >= 0:
            bit = 1
        else:
            bit = 0
            
        bits = (bits << 1) | bit
        
    return bits

# Initialize the accumulator as a global (simulating static behavior)
accumulator = 0

# Example usage:
sample_value = 32768  # Mid-level input
pdm_word = generate_pdm_word_16bit(sample_value)
print(f"PDM Word: {pdm_word:032b}")