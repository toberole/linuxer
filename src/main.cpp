#include <iostream>
#include <thread>
#include <mutex>
#include <string>

void test(){
    std::cout<<"--- test ---"<<std::endl;
}

void test0(){
    std::thread th1(test);
    std::thread th2(test);

    th1.join();
    th2.join();
}

void test1(std::mutex &mtx, int &count){
    
    while(count<100){
        std::lock_guard<std::mutex> m(mtx);
        if(count<100){
            std::cout<<"thread_ID: "<<std::this_thread::get_id()<<" count = "<<count<<"\n";
            count++;
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    }
}

void test_1(){
    std::mutex mtx;
    int count = 0;
    std::thread * ths[10];
    for(int i =0;i<5;i++){
        ths[i] = new std::thread(test1,std::ref(mtx),std::ref(count));
    }
    
    for(auto th:ths)
    {
       (*th).join();
    }

    
    for(auto th:ths)
    {
        delete th;
    }

}

int main(int argc, char const *argv[])
{
    
    
       
    std::cout<<"press any key to exit ......"<<std::endl;
    getchar();
    return 0;
}
