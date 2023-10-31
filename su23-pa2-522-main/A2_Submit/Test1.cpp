/*In this test, we want to test if our program can deal with loop.
 *We will start with a nested loop. 
 *Besides, we will combine loop and fuction call together to see if the program can output correct result.
 *We will also test the scenery when cin is within the loop.
 */
/*-------expected output----------
 *Line 24: x is tainted
 *Line 29: f is tainted
 *Line 33: e is tainted
 *Line 38: f is now untainted
 *Line 39: e is now untainted
 *Tainted: {x}
 */

#include<iostream>
using namespace std;

int add_func(int a, int b) {
   return a + b;
}

int main(){	
    int x, y;					    // Tainted = {}
    cin >> x;					    // Tainted = {x}
    int e;                                          // Tainted = {x}           
    int f;                                          // Tainted = {x}           
    for(int i = 0; i <= 5; i++){                    // Tainted = {x}          
        if( i == 3 ){                               // Tainted = {x}           
            f = add_func(x, 1);                     // Tainted = {x,f}         
	}                                           // Tainted = {x,f}         
	for(int j = 0; j<=3; j++){                  // Tainted = {x,f}         
	    if(j == 2){			            // Tainted = {x,f}	        
	        cin >> e;                           // Tainted = {x,e,f}	    
	    }                                       // Tainted = {x,e,f}	    
	} 	                        	    // Tainted = {x,e,f}	    
    }			                            // Tainted = {x,e,f}	    
    cout << f;                                      // Tainted = {x,e,f}	    
    f = add_func(1, 1);			            // Tainted = {x,e}        
    e = 0;                                          // Tainted = {x} 
}
