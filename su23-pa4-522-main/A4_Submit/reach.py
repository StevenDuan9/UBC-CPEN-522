from z3 import *

def main():
    a, b, c, d, e, f, r1, r2 = Reals('a b c d e f r1 r2')
    
    solver = Solver()

    e = 0
    d = b * b - 4 * a * c
    r1 = (-b + pow(d, 3)) / (4 * a)
    r2 = (-b - pow(d, 3)) / (2 * a)
    f = e * d + e
    
    # Add constraints based on the code logic
    solver.add(a != 0)
    solver.add(d > 0)
    #solver.add(r1 > r2)
    #solver.add(r1 == 0)
    
    # Check if the path leading to the assertion is reachable
    assertion1_reachable = solver.check()
    
    # Reset solver for the next assertion
    solver.reset()
    
    # Add constraints for the next assertion
    solver.add(a != 0)
    solver.add(d == 0)
    #solver.add(r1 != r2)
    
    # Check if the path leading to the assertion is reachable
    assertion2_reachable = solver.check()
    
    # Reset solver for the next assertion
    solver.reset()
    
    # Add constraints for the next assertion
    solver.add(a != 0)
    solver.add(d == 0)
    solver.add(r1 != r2)
    solver.add(b != 0)
    solver.add(c == 0)
    #solver.add(r1 == r2)

    # Check if the path leading to the assertion is reachable
    assertion3_reachable = solver.check()
    
    # Reset solver for the next assertion
    solver.reset()
    
    # Add constraints for the next assertion
    solver.add(a != 0)
    solver.add(d < 0)
    #solver.add(f > e)
    
    # Check if the path leading to the assertion is reachable
    assertion4_reachable = solver.check()
    
    print("Assert1 Path:", assertion1_reachable)
    print("Assert2 Path:", assertion2_reachable)
    print("Assert3 Path:", assertion3_reachable)
    print("Assert4 Path:", assertion4_reachable)

if __name__ == '__main__':
    main()
