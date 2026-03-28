**PROCEDURE: main
**BEGIN: Three Address Code Statements
max_ = 0
a_ = 5
b_ = 1
temp0 = a_ > max_
temp1 = ! temp0
if(temp1) goto Label1
max_ = a_
goto Label0
Label1:
temp2 = b_ > max_
temp3 = ! temp2
if(temp3) goto Label3
max_ = b_
goto Label2
Label3:
temp4 = c_ > max_
temp5 = ! temp4
if(temp5) goto Label5
max_ = c_
goto Label4
Label5:
Label4:
Label2:
Label0:
c_ = 2
**END: Three Address Code Statements
