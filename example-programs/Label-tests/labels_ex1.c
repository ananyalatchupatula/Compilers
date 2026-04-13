int i1,i2, a, st,stg;
int func1();
int apple();
int func11();
float f1,f2;
int func2(int a,float b,int c);
float func3(int p,float q,float r);

int func4(){
	i1=4;
	return i1+i1;
}

int apple(){
    i1 = 5;
    return i1;
}

int func11(){
    i1 = 5;
    while(i1 < 7){
        print i1;
        i1 = i1 + 1;
    }
    return 1;
}

void func1(){
	i2=5;
	f2=5.0;
    while(i2 < 7){
        print i2;
        i2 = i2 + 1;
    }
    return i2;
}

int func2(int a,float b,int i1){
	a=a+10;
	i1=5;
	return a+i1+i2+2;
}

float func3(int a,float b,float c){
	b =b+f2;
	c=c+10.5;
	return b+c;
}

void main(){
	a = func1();
	// print i2; print f2;
	i2 = func4();
	// print i1; print i2;
	i2 = func2(i1,f2,i2);
	// print i1; print i2;
	f2 = func3(i1,f1,f2);
    st = apple();
    stg = func11();
	print f1; print f2; print a; print st; print stg;

}
