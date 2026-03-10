#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#define SIGN (1u << ((sizeof(unsigned int)* 8)-1))
#define BASE (1ULL << 32)
typedef struct{
    int elder_number;
    unsigned int* digits;
}BigInt;

void init(BigInt* num){
    num->elder_number=0;
    num->digits=malloc(sizeof(unsigned int));
    if(num->digits==NULL){
        num->digits=NULL;
        return;
    }
    num->digits[0]=0;
}

void freee(BigInt* num){
    num->elder_number=0;
    free(num->digits);
    num->digits=NULL;
}

unsigned long long get_abs_elder(BigInt* num) {
    return (num->elder_number < 0) ? -(long long)num->elder_number : num->elder_number;
}

unsigned int privet(BigInt* num){
    return num->elder_number & (~SIGN);
}

int get_sign(BigInt* num){
    if(num->digits[0]==0 && num->elder_number==0){
        return 0;
    }
    return (num->elder_number<0) ? -1 : 1;
}

void copy1(BigInt* num1,BigInt* num2){
    unsigned int len=num2->digits[0];
    num1->digits=malloc((len+1)*sizeof(unsigned int));
    if(num1->digits==NULL){
        return;
    }
    memcpy(num1->digits,num2->digits,(len+1)*sizeof(unsigned int));
    num1->elder_number=num2->elder_number;
}

int modul(BigInt* num1,BigInt* num2){
    unsigned int e1=get_abs_elder(num1);
    unsigned int e2=get_abs_elder(num2);
    unsigned int len1=num1->digits[0];
    unsigned int len2=num2->digits[0];
    unsigned int total1=(e1==0 && len1==0) ? 0 : len1+1;
    unsigned int total2=(e2==0 && len2==0) ? 0 : len2+1;
    if(total1!=total2){
        return (total1>total2) ? 1 : -1;
    }
    if(e1!=e2){
        return (e1>e2) ? 1 : -1;
    }
    for(unsigned int i=len1;i>=1;i--){
        if(num1->digits[i]!=num2->digits[i]){
            return num1->digits[i]>num2->digits[i] ? 1 : -1;
        }
    }
    return 0;
}

void plus(BigInt* num1,BigInt* num2, int sign_num2){
    if(num2->digits[0]==0 && num2->elder_number==0){
        return;
    }
    int sign_num1=get_sign(num1);
    if(num1->digits[0]==0 && num1->elder_number==0){
        free(num1->digits);
        copy1(num1,num2);
        if(sign_num2<0){
            num1->elder_number=-num1->elder_number;
        }
        return;
    }
    if (sign_num1==sign_num2){
        unsigned int high_num1=get_abs_elder(num1);
        unsigned int high_num2=get_abs_elder(num2);
        unsigned int len1=num1->digits[0];
        unsigned int len2=num2->digits[0];
        unsigned int max_len=(len1>len2) ? len1 : len2;
        unsigned int* tmp=calloc(max_len+1,sizeof(unsigned int));
        if(tmp==NULL){
            return;
        }
        unsigned long long carry=0;
        for(unsigned int i=0;i<max_len;i++){
            unsigned long long sum=carry;
            if(i<len1){
                sum+=num1->digits[i+1];
            }
            if(i<len2){
                sum+=num2->digits[i+1];
            }
            tmp[i]=(unsigned int)sum;
            carry=sum >> 32;
        }
        unsigned long long sum_high=high_num1+high_num2+carry;
        unsigned int new_len;
        unsigned int* new_digits;
        long long new_high;
        if(sum_high<BASE){
            new_high=(long long)sum_high;
            new_len=max_len;
            new_digits=malloc((new_len+1)*sizeof(unsigned int));
            if(new_digits==NULL){
                free(tmp);
                return;
            }
            for(unsigned int i=0;i<max_len;i++){
                new_digits[i+1]=tmp[i];
            }
            new_digits[0]=new_len;
        }else{
            unsigned long long high_part=sum_high>>32;
            unsigned long long low_part=sum_high & 0xFFFFFFFF;
            new_len=max_len+1;
            new_high=high_part;
            new_digits=malloc((new_len+1)*sizeof(unsigned int));
            if(new_digits==NULL){
                free(tmp);
                return;
            }
            new_digits[0]=new_len;
            for(unsigned int i=0;i<max_len;i++){
                new_digits[i+1]=tmp[i];
            }
            new_digits[new_len]=(unsigned int)low_part;
        }
        if(new_high>INT_MAX || new_high<INT_MIN){
            free(tmp);
            free(new_digits);
            return;
        }
        int final_high=(int)new_high;
        if(sign_num1<0){
            final_high=-final_high;
        }
        free(num1->digits);
        num1->digits=new_digits;
        num1->elder_number=final_high;
        free(tmp);
    }else{
        int res=modul(num1,num2);
        BigInt* big;
        BigInt* small;
        int res_sign=0;
        if(res==0){
            free(num1->digits);
            num1->digits=malloc(sizeof(unsigned int));
            if(num1->digits==NULL){
                return;
            }
            num1->digits[0]=0;
            num1->elder_number=0;
            return;
        }
        else if(res==1){
            big=num1;
            res_sign=sign_num1;
            small=num2;
        }
        else{
            res_sign=sign_num2;
            big=num2;
            small=num1;
        }
        unsigned int big_len=big->digits[0];
        unsigned int small_len=small->digits[0];
        unsigned int* tmp=calloc(big_len+1,sizeof(unsigned int));
        if(tmp==NULL){
            return;
        }
        unsigned int big_elder=privet(big);
        unsigned int small_elder=privet(small);
        unsigned int* new_big=(unsigned int*)malloc((big_len+1)*sizeof(unsigned int));
        if(new_big==NULL){
            free(tmp);
            free(new_big);
            return;
        }
        unsigned int* new_small=(unsigned int*)malloc((small_len+1)*sizeof(unsigned int));
        if(new_small==NULL){
            free(tmp);
            free(new_big);
            free(new_small);
            return;
        }
        new_small[small_len]=small_elder;
        new_big[big_len]=big_elder;
        unsigned long long carry=0;
        for (int i = 0; i < big_len; i++) new_big[i] = big->digits[i+1];
        new_big[big_len] = big_elder;
        for(int i=0; i<small_len;i++) new_small[i]=small->digits[i+1];
        for(int j=0;j<big_len+1;j++){
            unsigned long long dif=carry;
            if(j<small_len){
                dif+=new_small[j];
            }
            else if(j==small_len){
                dif+=new_small[small_len];
            }
            unsigned long long big_value=new_big[j];
            if(big_value>=dif){
                tmp[j]=(unsigned int)big_value-dif;
                carry=0;
            }else{
                tmp[j]=(unsigned int)(big_value+BASE-dif);
                carry=1;
            }
        }
        unsigned int r=big_len+1;
        while(r>1 && tmp[r-1]==0){
            r-=1;
        }
        if(r==0){
            free(num1->digits);
            init(num1);
            free(tmp);
            free(new_big);
            free(new_small);
            return;
        }
        unsigned int new_len=r-1;
        unsigned long long abs_elder = tmp[r - 1];
        num1->elder_number = (res_sign < 0) ? -(long long)abs_elder : (long long)abs_elder;
        free(num1->digits);
        unsigned int* new_digits=(unsigned int*)malloc((new_len+1)*sizeof(unsigned int));
        if(new_digits==NULL){
            return;
        }
        new_digits[0]=new_len;
        for(int i=0;i<new_len;i++){
            new_digits[i+1]=tmp[i];
        }
        free(tmp);
        free(new_big);
        free(new_small);
        num1->digits=new_digits;
    }
}

void BigIntPlus(BigInt* num1,BigInt* num2){
    plus(num1,num2,get_sign(num2));
}

void BigIntMinus(BigInt* num1,BigInt* num2){
    plus(num1,num2,-get_sign(num2));
}

BigInt BigIntPlusNew(BigInt* num1,BigInt* num2){
    BigInt res;
    copy1(&res,num1);
    BigIntPlus(&res,num2);
    return res;
}

BigInt BigIntMinusNew(BigInt* num1,BigInt* num2){
    BigInt res;
    copy1(&res,num1);
    BigIntMinus(&res,num2);
    return res;
}

void multi(BigInt* num1,BigInt* num2){
    if((num1->digits[0]==0 && num1->elder_number==0) || (num2->elder_number==0 && num2->digits[0]==0)){
        freee(num1);
        init(num1);
        return;
    }
    int sign_num1=get_sign(num1);
    int sign_num2=get_sign(num2);
    int result_sign=(sign_num1==sign_num2) ? 1 : -1;
    unsigned int len_num1=num1->digits[0]+1;
    unsigned int len_num2=num2->digits[0]+1;
    unsigned int res_len=len_num1+len_num2+1;
    unsigned int* tmp=calloc(res_len,sizeof(unsigned int));
    if(tmp==NULL){
        return;
    }
    unsigned int* num1_full=(unsigned int*)malloc((len_num1)*sizeof(unsigned int));
    if(num1_full==NULL){
        free(tmp);
        return;
    }
    unsigned int* num2_full=(unsigned int*)malloc((len_num2)*sizeof(unsigned int));
    if(num2_full==NULL){
        free(tmp);
        free(num1_full);
        return;
    }
    for(int i=0;i<len_num1-1;i++){
        num1_full[i]=num1->digits[i+1];
    }
    num1_full[len_num1-1]=num1->elder_number;
    for(int i=0;i<len_num2-1;i++){
        num2_full[i]=num2->digits[i+1];
    }
    num2_full[len_num2-1]=num2->elder_number;
    for(int i=0;i<len_num1;i++){
        unsigned long long carry=0;
        unsigned long long value=num1_full[i];
        for(int j=0;j<len_num2;j++){
            unsigned long long value1=num2_full[j];
            unsigned long long res=value1*value+carry+tmp[i+j];
            tmp[i+j]=res % 4294967296ULL;
            carry=res / 4294967296ULL;
        }
        unsigned int pos=i+len_num2;
        while(carry){
            if(pos>=res_len){
                unsigned int* new_tmp=realloc(tmp,(res_len+1)*sizeof(unsigned int));
                if(new_tmp==NULL){
                    free(tmp);
                    free(num1_full);
                    free(num2_full);
                    return;
                }
                tmp=new_tmp;
                res_len++;
            }
            unsigned long long sum=(unsigned long long)tmp[pos]+carry;
            tmp[pos]=(unsigned int)sum;
            carry=sum>>32;
            pos++;
        }
    }
    unsigned int j=res_len;
    while(j>0 && tmp[j]==0){
        j-=1;
    }
    if(j==0){
        free(num1->digits);
        init(num1);
        free(tmp);
        free(num1_full);
        free(num2_full);
    }
    free(num1->digits);
    unsigned int* new_digits=(unsigned int*)malloc((j+1)*sizeof(unsigned int));
    new_digits[0]=j;
    for(int i=0;i<j-1;i++){
        new_digits[i+1]=tmp[i];
    }
    num1->elder_number=tmp[j-1]*result_sign;
    free(tmp);
    free(num1_full);
    free(num2_full);
}

BigInt mult(BigInt* num1,BigInt* num2){
    BigInt res;
    copy1(&res,num1);
    multi(&res,num2);
    return res;
}

void print(BigInt* num){
    if(num->digits[0]==0 && num->elder_number==0){
        printf("0");
        return;
    }
    else if(num->digits[0]==0 && num->elder_number!=0){
        if(get_sign(num)<0){
            printf("-");
        }
        printf("%x",(num->elder_number<0) ? -num->elder_number : num->elder_number);
        return;
    }
    if(get_sign(num)<0){
        printf('-');
    }
    unsigned int len=num->digits[0];
    int n=num->elder_number;
    printf("%x",(n<0) ? -n : n);
    for(int i=len;i>=1;i--){
        printf("%08x",num->digits[i]);
    }
    return;
}

void add(BigInt* num,int value){
    if(value==0){
        free(num->digits);
        init(num);
        return;
    }
    free(num->digits);
    unsigned int* new_digits=(unsigned int*)malloc(sizeof(unsigned int));
    new_digits[0]=0;
    num->elder_number=value;
}

BigInt vosstanovlenie(unsigned int* arr[],unsigned int len){
    BigInt res;
    if(len==0){
        init(&res);
        return res;
    }
    unsigned int j=len;
    while(j>0 && arr[j-1]==0){
        j-=1;
    }
    if(j==0){
        init(&res);
        return res;
    }
    unsigned int new_len=j-1;
    unsigned int* new_digits=(unsigned int*)malloc((new_len+1)*sizeof(unsigned int));
    if(new_digits==NULL){
        init(&res);
        return;
    }
    new_digits[0]=new_len;
    for(int i=0;i<new_len;i++){
        new_digits[i+1]=arr[i];
    }
    res.digits=new_digits;
    res.elder_number=(int)arr[len-1];
    return;
} 

BigInt sdvig(BigInt* num,unsigned int shift){
    BigInt res;
    if(shift==0){
        init(&res);
        return res;
    }
    unsigned int old_len=num->digits[0];
    unsigned int new_len=old_len+shift;
    unsigned int* new_digits=(unsigned int*)malloc((new_len+1)*sizeof(unsigned int));
    if(new_digits==NULL){
        init(&res);
        return res;
    }
    new_digits[0]=new_len;
    for(int i=1;i<shift;i++){
        new_digits[i]=0;
    }
    for(int i=1;i<old_len;i++){
        new_digits[i+shift]=num->digits[i];
    }
    res.digits=new_digits;
    res.elder_number=num->elder_number;
    return res;
}

BigInt karatsuba_absolute(BigInt* num1,BigInt* num2){
    unsigned int len1=num1->digits[0]+1;
    unsigned int len2=num2->digits[0]+1;
    if(len1<=2 && len2<=2){
        return mult(num1,num2);
    }
    unsigned int n=(len1>len2) ? len1 : len2;
    unsigned int* num1_full=(unsigned int*)calloc(n,sizeof(unsigned int));
    unsigned int* num2_full=(unsigned int*)calloc(n,sizeof(unsigned int));
    if(num1_full==NULL || num2_full==NULL){
        free(num1_full);
        free(num2_full);
        BigInt zero;
        init(&zero);
        return zero;
    }
    for(int i=0;i<len1-1;i++){
        num1_full[i]=num1->digits[i+1];
    }
    num1_full[len1-1]=(num1->elder_number<0) ? -num1->elder_number : num1->elder_number;
    for(int i=0;i<len2-1;i++){
        num2_full[i]=num2->digits[i+1];
    }
    num2_full[len2-1]=(num2->elder_number<0) ? -num2->elder_number : num2->elder_number;
    unsigned int m=n/2;
    BigInt a0=vosstanovlenie(num1_full,m);
    BigInt a1=vosstanovlenie(num1_full+m,n-m);
    BigInt b0=vosstanovlenie(num2_full,m);
    BigInt b1=vosstanovlenie(num2_full+m,n-m);
    free(num1_full);
    free(num2_full);
    BigInt p2=karatsuba_absolute(&a0,&b0);
    BigInt p1=karatsuba_absolute(&a1,&b1);
    BigInt a0_plus_a1=BigIntPlusNew(&a0,&a1);
    BigInt b0_plus_b1=BigIntPlusNew(&b0,&b1);
    BigInt p3=karatsuba_absolute(&a0_plus_a1,&b0_plus_b1);
    freee(&a0);
    freee(&b0);
    freee(&a1);
    freee(&b1);
    freee(&a0_plus_a1);
    freee(&b0_plus_b1);
    BigInt first_minus=BigIntMinusNew(&p3,&p2);
    BigInt second_minus=BigIntMinusNew(&first_minus,&p1);
    freee(&first_minus);
    freee(&p3);
    BigInt p1_shift=sdvig(&p1,2*m);
    BigInt second_minus_shift=sdvig(&second_minus,m);
    freee(&p1);
    freee(&second_minus);
    BigInt res0=BigIntPlusNew(&p1_shift,&second_minus_shift);
    freee(&p1_shift);
    freee(&second_minus_shift);
    BigInt res=BigIntPlusNew(&res0,&p2);
    freee(&res0);
    freee(&p2);
    return res;
}

void karatsuba(BigInt* num1,BigInt* num2){
    if((num1->digits[0]==0 && num1->elder_number==0) || (num2->digits[0]==0 && num2->elder_number==0)){
        free(num1);
        init(num1);
        return;
    }
    int sign=get_sign(num1);
    int sign2=get_sign(num2);
    int res_sign=(sign==sign2) ? 1 : -1;
    BigInt absa,absb;
    copy1(&absa,num1);
    copy1(&absb,num2);
    absa.elder_number=(num1->elder_number<0) ?-num1->elder_number : num1->elder_number;
    absb.elder_number=(num2->elder_number<0) ?-num2->elder_number : num2->elder_number;
    BigInt res_abs=karatsuba_absolute(&absa,&absb);
    if(res_sign<0 && (res_abs.digits[0]!=0 || res_abs.elder_number!=0)){
        res_abs.elder_number=-res_abs.elder_number;
    }
    freee(num1);
    *num1=res_abs;
    freee(&absa);
    freee(&absb);
}

BigInt function1(BigInt* num){
    BigInt res;
    if(num->digits[0]==0 && num->elder_number==0){
        init(&res);
        return res;
    }
    unsigned int len=num->digits[0];
    unsigned int* num1_full=(unsigned int*)malloc((len+1)*sizeof(unsigned int));
    for(int i=0;i<len-1;i++){
        num1_full[i]=num->digits[i+1];
    }
    num1_full[len-1]=num->elder_number;
    if(num1_full[0]%2==0){
        init(&res);
        return res;
    }
    else{
        BigInt* current_factorial;
        init(current_factorial);
        current_factorial->elder_number=1;
        BigInt i;
        init(&i);
        i.elder_number=1;
        BigInt* one;
        init(one);
        one->elder_number=1;
        while(modul(num,&i)>=0){
            mult(current_factorial,&i);
            i=BigIntPlusNew(&i,one);
        }
        return *current_factorial;
    }
}

BigInt function2(BigInt* num){
    
}

int main(){
    BigInt a,b,c,c1,c2;
    init(&a);
    init(&b);
    add(&a,123456789);
    add(&b,987654321);
    c=BigIntPlusNew(&a,&b);
    c1=BigIntMinusNew(&a,&b);
    c2=mult(&a,&b);
    print(&c);
    printf("\n");
    print(&c1);
    printf("\n");
    print(&c2);
    printf("\n");
    freee(&a);
    freee(&b);
    freee(&c);
    freee(&c1);
    freee(&c2);
}

