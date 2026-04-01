**PROCEDURE: main
**BEGIN: Three Address Code Statements
Label2:
Label0:
temp0 = i_ > 10
temp2 = ! temp0
if(temp2) goto Label1
temp1 = i_ + 1
i_ = temp1
goto Label0
Label1:
if(x_) goto Label2
temp3 = i_ - 3
i_ = temp3
**END: Three Address Code Statements
