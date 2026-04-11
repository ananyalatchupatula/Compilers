**PROCEDURE: main
**BEGIN: Three Address Code Statements
n_ = 5
fact_ = 1
c_ = 1
Label0:
temp0 = c_ <= n_
temp3 = ! temp0
if(temp3) goto Label1
temp1 = fact_ * c_
fact_ = temp1
temp2 = c_ + 1
c_ = temp2
goto Label0
Label1:
**END: Three Address Code Statements
