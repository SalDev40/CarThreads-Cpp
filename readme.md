Description:
    
    - A car tunnel is very poorly ventilated, these restrictions must be enforced

        1.The number of northbound cars in the tunnel
        2.The number of southbound cars in the tunnel
        3.The total number of cars in the tunnel.

    - main thread - will fork a tunnel process and the  car  processes  according  to  the  input specifications.

    - one car thread  per car wanting to cross the tunnel.


Input: 

    - Input to your program consists of the maximum number of northbound cars in the tunnel, the maximum number of southbound cars , and the maximum total number of cars in the tunnel 


How To Use:

    - USE WSL, MacOS, Linux for IO/redirection
    - g++ threads.cpp -o threads -lpthread
    - ./threads < input33.txt
                - threre are 4 input files to test 
