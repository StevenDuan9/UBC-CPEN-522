/*In this test, we want to test if our program can deal with conditionals.
 *Since we have already tested simple conditional , in this case we will start with a nested conditional.
 *After that, we will combine conditionals and fuction call together to see if the program can output correct result.
 *Finally, we will also test the scenery when cin is within the conditionals.
 */
/*-------expected output----------
 *Line 26: x is tainted
 *Line 30: a is tainted
 *Line 31: b is tainted
 *Line 39: b is now untainted
 *Line 40: a is now untainted
 *Tainted: {x}
 */

#include <iostream>
using namespace std;

int add_func(int a, int b) {
   return a + b;
}
int main(){
    // test nested conditionals
    // test function call within nested conditonals
    // test cin within nested conditionals  
    int x,a,b;                                      // Tainted = {}
    cin >> x;                                       // Tainted = {x}
    int f = 0;		 			    // Tainted = {x}
    if(x > a){                                      // Tainted = {x}
	if(x > b){                                  // Tainted = {x}
            cin >> a;				    // Tainted = {x,a}
            b = x;                                  // Tainted = {x,a,b}
	}else{				            // Tainted = {x}
            b = add_func(x, f);			    // Tainted = {x,b}
	}                                           // Tainted = {x,b}                 	  
    }else{					    // Tainted = {x}
	a = 2;                                      // Tainted = {x}
    }                                               // Tainted = {x}
    cout << b;                                      // Tainted = {x,a,b}
    b = 0;                                          // Tainted = {x,a}
    a = add_func(1, 1);                             // Tainted = {x}
    return 0;
}
