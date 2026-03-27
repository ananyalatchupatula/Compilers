**PROCEDURE: main
**BEGIN: Three Address Code Statements
n_ = 3
temp0 = n_ / 2
temp1 = temp0 * 2
temp2 = temp1 == n_
temp3 = ! temp2
if(temp3) goto Label1
cntr_ = 1
goto Label0
Label1:
cntr_ = 0
Label0:
**END: Three Address Code Statements
