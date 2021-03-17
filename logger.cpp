/*
File:logger.cpp
Utilities for logging and struct definition for thread argument
Author:Kushagra Indurkhya
*/
#include<string>
#include<iostream>
#include<fstream>
#include<thread>
#include<vector>
#include<algorithm>
#include<sys/time.h>
#include<chrono>

/*
Class Logger
Providing logging utillities

Members:

int hr,min,sec,ms : local time precise to milliseconds when object is created
epoch : is the number of milliseconds that have elapsed since January 1, 1970 (GMT)
int local_id : index of thread in thread_arr
string action : action which triggered the creation (request/entry/exit)

*/

class logger
{
    public:
        int hr,min,sec,ms;
        long epoch;

        int local_id;
        int iter;
        std::string action,algo;

        //Constructor
        logger(int i,int l_id,std::string act)
        {
            local_id=l_id;
            iter=i;
            action = act;


            struct timeval time_now = {0};
            long time_mil;
            long a,b;
            //Getting current time
            gettimeofday(&time_now,NULL);
            //Converting to milliseconds
            time_mil=(time_now.tv_sec*1000 + time_now.tv_usec/1000);
            epoch = time_mil;


            a = (time_mil%(1000*60*60*24))/3600000; //converting to hrs
            b = (time_mil%(1000*60*60))/60000; //converting to mins

            //adjusting for GMT for IST +5:30
            hr=a+5+(b+30)/60;
            min=(b+30)%60;

            sec=(time_mil%(1000*60))/1000;
            ms=time_mil%(1000);
        }

        //Returns log string
        std::string fetch_log()
        {
            std::string time_str=std::to_string(hr)+":"+std::to_string(min)+":"+std::to_string(sec)+":"+std::to_string(ms);
            std::string res = std::to_string(iter)+"th CS "+action+"at "+time_str+" by thread-"+std::to_string(local_id)+"\n";
            return res;
        }

        //Returns epoch difference between two objects
        long difference(logger b)
        {
            return (hr-b.hr)*3600000+(min-b.min)*60000+(sec-b.sec)*1000+(ms-b.ms);
        }
};

/*

Struct to be passed in thread funtion

local_id :  index of thread in thread_arr
vector<logger> buffer : vector of logger objects
vector<int> waiting_time : vector of waiting times of all the CS requests made in that thread
worst_waiting_time : max waiting time encountered in that thread

*/

typedef struct thread_arg{
  int local_id;
  std::vector<logger> buffer;
  std::vector<int> waiting_time;
  int worst_waiting_time;

} t_arg;


/*Comparator for sort*/
bool compare_log_obj(logger l1,logger l2)
{

    /*if epoch of both is not equal return one with less epoch time (ie occured first)*/
    if(l1.epoch != l2.epoch)
        return (l1.epoch<l2.epoch);
    /*return one with smaller local_id if epoch is equal*/
    else 
    // if(l1.local_id != l2.local_id)
        return(l1.local_id<l2.local_id);
}