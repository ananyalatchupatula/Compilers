**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 5
b_ = 1
Label2:
temp0 = a_ < 10
temp5 = ! temp0
if(temp5) goto Label3
temp1 = a_ + 1
a_ = temp1
Label0:
temp2 = b_ < 5
temp4 = ! temp2
if(temp4) goto Label1
temp3 = b_ + 1
b_ = temp3
a_ = 2
goto Label0
Label1:
goto Label2
Label3:
**END: Three Address Code Statements
