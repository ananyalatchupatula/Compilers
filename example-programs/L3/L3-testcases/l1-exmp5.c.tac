**PROCEDURE: main
**BEGIN: Three Address Code Statements
read local_
global1_ = 5
global2_ = local_
temp0 = global2_ > global1_
temp1 = global1_ / 2
temp2 = temp1 != 0
temp3 = temp0 && temp2
temp4 = ! temp3
if(temp4) goto Label0
stemp0 = 1
goto Label1
Label0:
stemp0 = 0
Label1:
ans_ = stemp0
write ans_
**END: Three Address Code Statements
