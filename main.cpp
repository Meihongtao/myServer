#include "./pool/threadsPool.h"
#include <chrono>
          
void func(int i,int b){
    int count = 0;
    for(int j=0;j<1000000;j++){
        count += j;
    }
}



int main(){
    ThreadPool *pool = new ThreadPool(3);
    for(int i=0;i<100;i++){
        pool->addTask(func,i+1,i+2);
    }
    std::cin.get();
}