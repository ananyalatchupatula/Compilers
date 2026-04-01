**PROCEDURE: main
**BEGIN: Three Address Code Statements
a_ = 5
b_ = 1
Label1:
Label0:
temp0 = b_ + 1
b_ = temp0
temp1 = b_ < 5
if(temp1) goto Label0
temp2 = a_ + 1
a_ = temp2
temp3 = a_ > 10
if(temp3) goto Label1
**END: Three Address Code Statements
