from z3 import *

def solve_assertion(assertion_p, assertion_q, constraints, variables):
    solver = Solver()

    solver.add(constraints)
    solver.add(And(assertion_p, assertion_q))
    if solver.check() == sat:
        model = solver.model()
        true_values = {str(variable): model[variable] for variable in variables}
        
    solver.reset()

    solver.add(constraints)
    solver.add(And(assertion_p, Not(assertion_q)))
    if solver.check() == sat:
        model = solver.model()
        false_values = {str(variable): model[variable] for variable in variables}
            
    return true_values, false_values
    # return None, None

def main():
    a, b, c,  = Reals('a b c')

    e = 0
    d = b * b - 4 * a * c
    r1 = (-b + pow(d, 3)) / (4 * a)
    r2 = (-b - pow(d, 3)) / (2 * a)
    f = e * d + e
    
    assertion1_p = And(a != 0, d > 0)
    assertion1_q = And(r1 > r2, r1 == 0)   

    assertion2_p = And(a != 0, d == 0)
    assertion2_q = And(r1 != r2)
    
    assertion4_p = And(a != 0, d < 0)
    assertion4_q = And(f > e)
    
    assertion_p = [assertion1_p, assertion2_p, assertion4_p]
    assertion_q = [assertion1_q, assertion2_q, assertion4_q]

    # assertions = [And(assertion1_p, assertion1_q), And(assertion2_p, assertion2_q), And(assertion4_p, assertion4_q)]
    assertion_names = ["Assert1", "Assert2",  "Assert4"]
    
    variables = [a, b, c]
    
    for name,p,q in zip(assertion_names, assertion_p, assertion_q):
        print(f"{name}:", end=' ')
        
        solver = Solver()
        solver.add(And(p, q))
        
        if solver.check() == sat:
            print("sat")
            true_values, false_values = solve_assertion(p, q, True, variables)
            
            print("Assert True Case:")
            print("[", end = "")
            if true_values:
                for variable, value in true_values.items():
                    print(f"{variable} = {value}" + ",")
                 
            print("..]")
            
            print("Assert False Case:")
            print("[", end = "")
            if false_values:
                for variable, value in false_values.items():
                    print(f"{variable} = {value}" + ",")
            print("..]")
        else:
            print("unsat")

if __name__ == '__main__':
    main()
