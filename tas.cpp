/*
File:tas.cpp
Implementing test and switch
Author:Kushagra Indurkhya
*/
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include"logger.cpp"
using namespace std;

//Global variables
atomic_flag lock = ATOMIC_FLAG_INIT;
int n,k;
double l1,l2;

//Seed for random value generator
long SEED;

void test_TAS(t_arg* data)
{
    // thread::id t_id = this_thread::get_id();

    default_random_engine generator(SEED);
    exponential_distribution<double> critical_section_time(1/l1);
    exponential_distribution<double> remaining_section_time(1/l2);

    int local_id = data->local_id;
    for(int i=0;i<k;i++)
    {
      /*entry section starts*/
      logger* r_log=new logger(i,local_id,"request");
      data->buffer.push_back(*r_log);
      while (lock.test_and_set()) {}
      /*entry section ends*/


      /*Critical section starts*/
      logger* entry_log=new logger(i,local_id,"entry   ");
      data->buffer.push_back(*entry_log);

      long curr_wait=entry_log->epoch-r_log->epoch;
      data->waiting_time.push_back(curr_wait);
      if(curr_wait > data->worst_waiting_time) 
        data->worst_waiting_time=curr_wait;



      this_thread::sleep_for(chrono::duration<double>(critical_section_time(generator))/1000);
      /*Critical section ends*/


      /*exit section starts*/
      logger* exit_log=new logger(i,local_id,"exit    ");
      data->buffer.push_back(*exit_log);
      lock.clear();
      /*exit section ends*/

      //Remainder Section
      this_thread::sleep_for(chrono::duration<double>(remaining_section_time(generator))/1000);
    }
}
int main ()
{
  //Giving epoch to random generator seed for uniqueness
  SEED = std::chrono::system_clock::now().time_since_epoch().count();
  
  vector<thread> threads;

  //Opening input file
  ifstream infile("inp-params.txt");
  if (!infile.is_open())
  {
      cout << "Unable to open input file\n";
      exit(1);
  }

  //Taking input for n,k,lambda1,lambda2
  infile >>n>>k>>l1>>l2;

  //Array of structs to be passed as argument to thread func
  t_arg a[n];

  //Creating n threads
  for (int i=0; i<n ;i++)
  {
    a[i].local_id=i;
    a[i].worst_waiting_time=-1;
    threads.push_back(thread(test_TAS,&a[i]));

  }

  //Joining n threads
  for (auto& th : threads) th.join();


  double total=0;
  int max_wait=-1;


  vector<logger> complete_log;

  for (size_t i = 0; i < n; i++)
  {
    //dumping thread buffer log to complete log
    for (logger w:a[i].buffer) complete_log.push_back(w);
    //clearing buffer vector
    a[i].buffer.clear();
    //summing waiting time
    for (int j:a[i].waiting_time) total+=j;
    //Cecking for max waiting time
    if(a[i].worst_waiting_time > max_wait) max_wait=a[i].worst_waiting_time;
  }

  
  // opening output file
  ofstream log_file("CAS_B-log.txt");
  if (!log_file.is_open())
  {
      cout << "Unable to generate output file\n";
      exit(1);
  }
  //sorting complete log and giving output to log file
  sort(complete_log.begin(),complete_log.end(),compare_log_obj);
  for (logger w:complete_log) log_file<<w.fetch_log();
  cout<<"Worst  Waiting time "<<max_wait<<endl;
  cout<<"Avg    Waiting time "<<total/(n*k)<<endl;


  
  return 0;
}