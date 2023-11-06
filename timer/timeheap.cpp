// #include<iostream>
// #include<chrono>
// #include<functional>
// #include<set>
// #include<sys/epoll.h>
// #include<memory>

// /*
// fuc 回调方法
// */

// struct TimerNodeBase
// {
//     /* data */
//     time_t expire;
//     int64_t id;
// };


// struct TimerNode : public TimerNodeBase{
//     using CallBack = std::function<void(const TimerNode &node)>;
//     CallBack func;
// };
 
// bool operator < (const TimerNodeBase &A,const TimerNodeBase &B){
//         if(A.expire < B.expire){
//             return true;
//         }
//         else if(A.expire >= B.expire){
//             return false;
//         }
//         return A.id < B.id;
//     }

// // 定时器类
// class Timer{
// public:
//     static time_t GetTick(){
//         auto sc = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
//         auto temp = std::chrono::duration_cast<std::chrono::milliseconds>(sc.time_since_epoch());
//         return temp.count();
//     }

//     static int GetID(){
//         return ++gid;
//     }

//     TimerNodeBase AddTimer(time_t msec,TimerNode::CallBack func){
//         // 添加到时间堆中
//         TimerNode tnode;
//         tnode.expire = GetTick()+msec;
//         tnode.id = GetID();
//         tnode.func = func;
//         timeMap.insert(tnode);
//         return static_cast<TimerNodeBase>(tnode);

//     }

//     bool DelTimer(TimerNodeBase &node){
//         // 删除
//         auto iter = timeMap.find(node);
//         if(iter != timeMap.end()){
//             timeMap.erase(iter);
//             return true;
//         }
//         return false;
//     }

//     bool CheckTimer(){
//         // 找到最小的节点
//         auto iter = timeMap.begin();
//         if(iter != timeMap.end() && iter->expire <= GetTick()){
//             iter->func(*iter);
//             timeMap.erase(iter);
//             return true;
//         }
//         return false;
//     }

//     time_t TimeToSleep(){
//         // 休眠多久
//         auto iter = timeMap.begin();
//         if(iter == timeMap.end()){
//             return -1;
//         }
//         time_t dis = iter->expire - GetTick();
//         return dis > 0 ? dis : 0;
//     }
// private:
//     static int64_t gid;
//     std::set<TimerNode,std::less<>> timeMap;

// };

// int64_t Timer::gid = 0;

// // int main(){
// //     int epfd = epoll_create(1);

// //     std::unique_ptr<Timer> timer = std::make_unique<Timer>();
// //     int i=0;
// //     timer->AddTimer(1000,[&](const TimerNode & node){
// //         std::cout << node.id << " " << ++i << std::endl;
// //     });

// //     timer->AddTimer(5500,[&](const TimerNode & node){
// //         std::cout << node.id << " " << ++i << std::endl;
// //     });

// //     epoll_event ev[64] = {0};

// //     while(1){
// //         int n = epoll_wait(epfd,ev,64,timer->TimeToSleep());
// //         for(int i =0;i<n;i++){

// //         }
// //         while(timer->CheckTimer());
// //     }

// //     return 0;
// // }