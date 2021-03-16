#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <unistd.h>
#include"log.hpp"
using namespace std;

atomic<bool> lock;
bool* waiting;
int n,k,t1,t2;


void test_CS_Bounded(t_arg* data)
{
    // thread::id t_id = this_thread::get_id();

    int local_id = data->local_id;
    for(int i=0;i<k;i++)
    {
        /*entry section starts*/
        log* r_log=new log(i,local_id,"request ","CAS_B");
        data->buffer.push_back(*r_log);


        waiting[local_id]=true;
        bool key=1;
        bool expected=false;
        while (waiting[local_id] && key==1)
        {
            key = !lock.compare_exchange_strong(expected,true);
            expected = false;
            sleep(0);
        }
        waiting[local_id]=false;
        /*entry section ends*/

        /*Critical section starts*/
        log* entry_log=new log(i,local_id,"entry   ","TAS");
        data->buffer.push_back(*entry_log);

        long curr_wait=entry_log->epoch-r_log->epoch;
        data->waiting_time.push_back(curr_wait);
        if(curr_wait > data->worst_waiting_time) 
          data->worst_waiting_time=curr_wait;


        this_thread::sleep_for(chrono::milliseconds(t1));
        /*Critical section ends*/


        /*exit section starts*/
        int j=(local_id+1)%n;

        while((j!=local_id) && !waiting[j])
            j=(j+1)%n;

        if(j == local_id)
        {
          log* exit_log=new log(i,local_id,"exit    ","TAS");
          data->buffer.push_back(*exit_log);

            lock=false;
        }
        else
        {
          log* exit_log=new log(i,local_id,"exit    ","TAS");
          data->buffer.push_back(*exit_log);

            waiting[j] = false;
        }
        /*exit section ends*/

        //Remainder Section
        sleep(0);
        this_thread::sleep_for(chrono::milliseconds(t2));
    }
    
}
int main ()
{
  vector<thread> threads;

  cin >>n>>k>>t1>>t2;

  t_arg a[n];

  bool wait_arr[n];
  waiting=&wait_arr[0];

  for (int i=0; i<n ;i++)
  {
    a[i].local_id=i;
    a[i].worst_waiting_time=-1;
    threads.push_back(thread(test_CS_Bounded,&a[i]));

  }

  for (auto& th : threads) th.join();

  double total=0;
  int max_wait=-1;

  vector<log> complete_log;
  for (size_t i = 0; i < n; i++)
  {
    for (log w:a[i].buffer) complete_log.push_back(w);
    a[i].buffer.clear();
    for (int i:a[i].waiting_time) total+=i;
    if(a[i].worst_waiting_time > max_wait) max_wait=a[i].worst_waiting_time;
  }

  sort(complete_log.begin(),complete_log.end(),compare_log_obj);
  ofstream log_file("CAS_B-log.txt");
  if (!log_file.is_open())
    {
        cout << "Unable to generate output file\n";
        exit(1);
    }

  sort(complete_log.begin(),complete_log.end(),compare_log_obj);
  for (log w:complete_log) log_file<<w.fetch_log();
  cout<<"Worst  Waiting time "<<max_wait<<endl;
  cout<<"Avg    Waiting time "<<total/n<<endl;


  
  return 0;
}