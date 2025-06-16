#!/usr/bin/env python3



#for i in range(16):
#   print(i, 0xFFFF >> i , "\t",16 - i, "bit resolution")
#-----------


def print_pwm(clk, bit_depth):
    
    right_shift = 16 - bit_depth
    rollover = 0xFFFF >> right_shift 
    
    rollover_freq = clk / rollover
    
    print(str(bit_depth).rjust(3),"bits - ", "rollover:", str(rollover).rjust(6), "   freq:", str(round(rollover_freq)).rjust(9))
    

SYSCLK = 150000000


def print_range_of_freqs(sysclk):
    
    print("\nPWM rollover rates at:", sysclk)
    print("--------------------------------")
    for i in range(10):
        print_pwm(sysclk, 16-i)
    

print_range_of_freqs(150000000)
print_range_of_freqs(300000000)
print_range_of_freqs(400000000)




'''
SYSCLK = 300000000
#SYSCLK = 400000000

ROLLOVER = 0xFFFF >> 6

rollover_freq = SYSCLK / ROLLOVER

print(rollover_freq)



#***  TURI SYNTH SETTINGS  

SYSCLK = 150000000
ROLLOVER = 2048                         #10 bits, ie 0xFFFF >> 6
rollover_freq = SYSCLK / ROLLOVER
print(rollover_freq)                    #  73khz




#***  TURI SYNTH SETTINGS  

SYSCLK = 400000000
ROLLOVER = 2048                         #10 bits, ie 0xFFFF >> 6
rollover_freq = SYSCLK / ROLLOVER
print(rollover_freq)                    # 195khz



#***  TURI SYNTH SETTINGS  

SYSCLK = 400000000
ROLLOVER = 0xFFFF >> 7                  #10 bits, ie 0xFFFF >> 6
rollover_freq = SYSCLK / ROLLOVER
print(rollover_freq)                    # 195khz


print(0xFFFF / 2048)
'''