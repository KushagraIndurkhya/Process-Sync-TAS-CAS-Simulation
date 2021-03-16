#include <atomic>
#include <thread>
#include <chrono>
#include"log.hpp"
using namespace std;

atomic_flag lock = ATOMIC_FLAG_INIT;
int n,k,t1,t2;

void testTS(t_arg* data)
{
    // thread::id t_id = this_thread::get_id();

    int local_id = data->local_id;
    for(int i=0;i<k;i++)
    {
      /*entry section starts*/
      log* r_log=new log(i,local_id,"request ","TAS");
      data->buffer.push_back(*r_log);
      while (lock.test_and_set()) {}
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
      log* exit_log=new log(i,local_id,"exit    ","TAS");
      data->buffer.push_back(*exit_log);
      lock.clear();
      /*exit section ends*/

      //Remainder Section
      this_thread::sleep_for(chrono::milliseconds(t2));
    }
}
int main ()
{
  vector<thread> threads;

  cin >>n>>k>>t1>>t2;

  t_arg a[n];
  
  for (int i=0; i<n ;i++) 
  {
    a[i].local_id=i;
    a[i].worst_waiting_time=-1;
    threads.push_back(thread(testTS,&a[i]));
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

  ofstream log_file("TAS-log.txt");
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