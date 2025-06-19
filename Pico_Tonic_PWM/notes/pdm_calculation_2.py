#!/usr/bin/env python3

def generate_pdm_word_16bit(value):
    """
    Generate a 32-bit PDM word from a 16-bit input value (0 to 65535),
    with debug printout of value, target, error, accumulator, and bit.
    """
    global accumulator

    bits = 0
    
    print(f"{'i':>2} {'value':>6} {'target':>7} {'error':>7} {'accumulator':>12} {'bit':>4}")
    print('-' * 44)
    
    for i in range(32):
        if accumulator >= 0:
            target = 65535
        else:
            target = 0
            
        error = value - target
        accumulator += error
        
        if accumulator >= 0:
            bit = 1
        else:
            bit = 0
            
        bits = (bits << 1) | bit
        
        # Print formatted debug output
        print(f"{i:2d} {value:6d} {target:7d} {error:7d} {accumulator:12d} {bit:4d}")
        
    return bits

# Initialize accumulator (simulating static variable)
# accumulator needs to be global (or static) for running calculations

accumulator = 0

# Example usage
sample_value = 40000  # Try values like 32768, 5000, 65535
pdm_word = generate_pdm_word_16bit(sample_value)

print(f"\nPDM Word (binary): {pdm_word:032b}")
print(f"PDM Word (hex):    0x{pdm_word:08X}")

