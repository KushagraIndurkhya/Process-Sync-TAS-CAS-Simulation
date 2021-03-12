#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <tuple>
#include <time.h>
#include <chrono>
using namespace std;
atomic_flag lock = ATOMIC_FLAG_INIT;
int k,t1,t2;

// tuple<clock_t,string> 

string currentDateTime()
{
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
    // make_tuple(clock(),buf);
}

void testTS(int local_id)
{
    thread::id t_id = this_thread::get_id();


    for(int i=0;i<k;i++)
    {
      auto request_entry_time = currentDateTime();
      

      while (lock.test_and_set()) {}
      cout << i << "th TS Request at " << request_entry_time  << " by thread-" << t_id << "-( " << local_id<< " )"<<endl;
      
      auto action_entry_time = currentDateTime();
      cout << i << "th TS Entry   at " << action_entry_time<< " by thread-" << t_id<< "-( " << local_id<< " )" <<endl;

      this_thread::sleep_for(chrono::milliseconds(t1*1000));

      auto exit_time = currentDateTime();
      cout << i << "th TS Exit    at " << exit_time << " by thread-" << t_id<< "-( " << local_id<< " )" <<endl;

      lock.clear();

      this_thread::sleep_for(chrono::milliseconds(t2*1000));
    }
}
int main ()
{
  vector<thread> threads;
  int n;
  cin >>n>>k>>t1>>t2;
  for (int i=1; i<=k ;i++) threads.push_back(thread(testTS,i));
  for (auto& th : threads) th.join();
  return 0;
}