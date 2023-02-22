#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

pqueue_arrival read_workload(string filename)
{
  pqueue_arrival workload;
  ifstream file;

  file.open(filename);
  if (file.is_open())
  {
    for (int i = 0; i < 3; i++)
    {
      Process p;

      file >> p.arrival >> p.duration; // p.arrival = file[0] p.duration = file[1] 
      p.first_run = -1;
      p.completion = -1;

      workload.push(p);
    }

    file.close();
  }

  return workload;
}

void show_workload(pqueue_arrival workload)
{
  pqueue_arrival xs = workload;
  cout << "workload:" << endl;
  while (!xs.empty())
  {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes)
{
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty())
  {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;

    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload)
{
  list<Process> complete;
  int counter = 0; // keeps track of completion time for processes

  while (!workload.empty())
  {
    Process p = workload.top(); // gets next element from queu
    workload.pop();
    p.first_run = counter; // for fifo a process runs when it arrives
    int i = p.duration;
    while (i > 0)
    {

      i--;

      counter++;
    }

    p.completion = counter;

    complete.push_back(p);
  }

  return complete;
}

list<Process> sjf(pqueue_arrival workload)
{

  list<Process> complete;

  int counter = 0;

  pqueue_duration arrived;

  while (!workload.empty())
  {

    Process p = workload.top();

    workload.pop();

    arrived.push(p);

    while (!workload.empty())
    {

      if (workload.top().arrival == p.arrival)
      {

        arrived.push(workload.top());

        workload.pop();
      }
      else
      {

        break;
      }
    }

    while (!arrived.empty())
    {

      Process q = arrived.top();

      arrived.pop();

      q.first_run = counter;

      int i = q.duration;

      while (i > 0)
      {

        i--;

        counter++;
      }

      q.completion = counter;

      complete.push_back(q);
    }
  }

  return complete;
}

list<Process> stcf(pqueue_arrival workload)
{

  list<Process> complete;

  pqueue_duration arrived;

  int counter = workload.top().arrival;

  while (!workload.empty())
  {

    Process p = workload.top();

    // Fills arrived with jobs that have same arrival time

    while (!workload.empty())
    {

      if (workload.top().arrival == p.arrival)
      {

        arrived.push(workload.top());

        workload.pop();
      }
      else
      {

        break;
      }
    }

    while (!arrived.empty())
    {

      Process q = arrived.top();

      // Avoids overrwriting first_run for a job we've already seen

      if (q.first_run == -1)
        q.first_run = counter;

      if (q.duration == 0)
      { // A job is finished

        q.completion = counter;

        complete.push_back(q);

        arrived.pop();
      }
      else
      {

        q.duration--;

        counter++;

        arrived.pop();

        arrived.push(q); // resinsert it back into the queue with less duration

        p = workload.top();

        if (p.arrival == counter)
        {

          // We must add all jobs that have same arrival time to arrived again

          while (!workload.empty())
          {

            if (workload.top().arrival == p.arrival)
            {

              arrived.push(workload.top());

              workload.pop();
            }
            else
            {

              break;
            }
          }
        }
      }
    }
  }

  return complete;
}

list<Process> rr(pqueue_arrival workload)
{

  list<Process> complete;

  int counter = 0;

  queue<Process> queue;

  while (!workload.empty())
  {

    queue.push(workload.top());

    workload.pop();
  }

  while (!queue.empty())
  {

    Process p = queue.front();

    if (p.first_run == -1)
      p.first_run = counter;

    if (p.duration == 0)
    {

      p.completion = counter++ - 2;

      complete.push_back(p);

      queue.pop();
    }
    else
    {

      p.duration--;

      counter++;

      queue.pop();

      queue.push(p);
    }
  }

  return complete;
}

float avg_turnaround(list<Process> processes)
{

  int sum = 0;

  for (auto i = processes.begin(); i != processes.end(); i++)
  {

    sum += (i->completion - i->arrival);
  }

  return (float)sum / (float)processes.size();
}

float avg_response(list<Process> processes)
{

  int sum = 0;

  for (auto i = processes.begin(); i != processes.end(); i++)
  {

    sum += (i->first_run - i->arrival);
  }

  return (float)sum / (float)processes.size();
}

void show_metrics(list<Process> processes)
{

  float avg_t = avg_turnaround(processes);

  float avg_r = avg_response(processes);

  show_processes(processes);

  cout << '\n';

  cout << "Average Turnaround Time: " << avg_t << endl;

  cout << "Average Response Time:   " << avg_r << endl;
}
