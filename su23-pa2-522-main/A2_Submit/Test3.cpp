/*In this test, we want to test if our program can deal with multiple sources of tainted data. 
 *The program is expected to handle tainted data in both branch or in straight-line code.
 *Besides, we will test if our program can deal with in-scope variables that are tainted.
 */
/*----------expected output-------------
 *Line 23: x is tainted
 *Line 24: y is tainted
 *Line 32: a is tainted
 *Line 38: y is now untainted
 *Line 39: x is now untainted
 *Tainted: {a}
 */

#include <iostream>
using namespace std;

int add_func(int a, int b) {
   return a + b;
}

int main(){
    int x,y,a;                 		// Tainted = {}
    cin >> x;                  		// Tainted = {x}
    cin >> y;                  		// Tainted = {x,y}
    int j = 0;			        // Tainted = {x,y}
    while(j < 20) {			// Tainted = {x,y}
    	int z;				// Tainted = {x,y}
	z = x;				// Tainted = {x,y}
    	if (x > y){                	// Tainted = {x,y}
            int b;                 	// Tainted = {x,y}
            cin >> b;              	// Tainted = {x,y}
	    a = b;                 	// Tainted = {x,y,a}
    	}	                       	// Tainted = {x,y,a}
    	j++;				// Tainted = {x,y,a}
    }					// Tainted = {x,y,a}
    x = y;                     		// Tainted = {x,y,a}
    y = x;                     		// Tainted = {x,y,a}
    y = 1;                     		// Tainted = {x,a}
    x = y;                     		// Tainted = {a}
    return 0;                           // Tainted = {a}
}
