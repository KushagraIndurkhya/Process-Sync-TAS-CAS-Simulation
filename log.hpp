#include<string>
#include<iostream>
#include<fstream>
#include<thread>
#include<vector>
#include<algorithm>
#include<sys/time.h>

class log
{
    public:
        int hr,min,sec,ms;
        long epoch;

        int local_id;
        int iter;
        std::string action,algo;

        log(int i,int l_id,std::string act,std::string al)
        {
            local_id=l_id;
            iter=i;
            action = act;
            algo = al;


            struct timeval time_now = {0};
            long time_mil;
            long a,b;

            gettimeofday(&time_now,NULL);

            time_mil=(time_now.tv_sec*1000 + time_now.tv_usec/1000);
            epoch = time_mil;

            a = (time_mil%(1000*60*60*24))/3600000;
            b = (time_mil%(1000*60*60))/60000;

            hr=a+5+(b+30)/60;;
            min=(b+30)%60;
            sec=(time_mil%(1000*60))/1000;
            ms=time_mil%(1000);
        }

        std::string fetch_log()
        {
            std::string time_str=std::to_string(hr)+":"+std::to_string(min)+":"+std::to_string(sec)+":"+std::to_string(ms);
            std::string res = std::to_string(iter)+"th "+algo+" "+action+"at "+time_str+" by thread-"+std::to_string(local_id)+"\n";
            return res;
        }

        

        long difference(log b)
        {
            return (hr-b.hr)*3600000+(min-b.min)*60000+(sec-b.sec)*1000+(ms-b.ms);
        }

};

typedef struct thread_arg{
  int local_id;
  std::vector<log> buffer;
  std::vector<int> waiting_time;
  int worst_waiting_time;

} t_arg;

bool compare_log_obj(log l1,log l2)
{
    if(l1.epoch != l2.epoch)
    {
        return (l1.epoch<l2.epoch);
    }
    else
    {
        return(l1.local_id<l2.local_id);
    }
}