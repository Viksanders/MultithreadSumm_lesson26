#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

void generate_random_vector(std::vector<long long>& in, int seed, unsigned long long size)
{
    srand(seed);
    //long long size = rand()%100000;
    in.clear();
    in.reserve(size);
    for (unsigned long long i = 0; i < size; ++i)
    {
        in.push_back(rand() % 100);
        //in.push_back(1);
    }
}

void print_vector(std::vector<long long>& in)
{
    for (size_t i = 0; i < in.size(); ++i)
    {
        std::cout << in[i] << " ";
    }
    std::cout << "\n";
}

void single_thread_summ(std::vector<long long>& in, int left, int right, long long& summ)
{
    //std::cout << "Started: " << std::this_thread::get_id() << "\n";
    for (int i = left; i <= right; ++i)
    {
        summ += in[i];
    }
    //std::cout << "Ended: " << std::this_thread::get_id() << "\n";
}

long long multi_thread_summ(std::vector<long long>& in, int left, int right, int threads_num)
{
    long long summ = 0;                         //общая сумма
    long long total_size = right - left + 1;  //количество элементов в заданном участке массива
    long long part_size = total_size / threads_num;    //размер одной части
    long long last_part_size = total_size % threads_num;    //размер последней части, если не делится нацело

    std::vector<std::thread> threads;

    int partial_size;
    if (last_part_size == 0) partial_size = threads_num;
    else partial_size = threads_num + 1;

    std::vector<long long> partial_summs(partial_size);
    for (int i = 0; i < threads_num; ++i)
    {
        int current_left = part_size * i;
        int current_right = (part_size * (i + 1)) - 1;
        //std::cout << "Thread # " << i << "  left = " << current_left << "    right = " << current_right << "\n";  //debug
        threads.push_back(std::thread(single_thread_summ, std::reference_wrapper<std::vector<long long> >(in), current_left, current_right, std::reference_wrapper<long long>(partial_summs[i])));
        //threads[i].join();    //если джойнить здесь, то многопоточности не будет, потоки будут выполняться друг за другом
    }

    //*
    for (int i = 0; i < threads_num; ++i)
    {
        threads[i].join();      //а если здесь, то потоки будут выполняться асинхронно
    }
    //*/

    if (last_part_size != 0)
    {
        int last_left = right - last_part_size + 1;
        //std::cout << "Last left: " << last_left << "\n";

        threads.push_back(std::thread(single_thread_summ, std::reference_wrapper<std::vector<long long> >(in), last_left, right, std::reference_wrapper<long long>(partial_summs[threads_num])));
        threads[threads_num].join();
    }


    single_thread_summ(partial_summs, 0, partial_summs.size() - 1, summ);
    return summ;
}


int main()
{
    std::vector<long long> v;
    long long summ = 0;
    //int threads_num = 5;
    int randomseed = 16;
    generate_random_vector(v, randomseed, 5000000);

    //print_vector(v);

    std::cout << "\n\n\n";
    auto start1 = std::chrono::high_resolution_clock::now();
    single_thread_summ(v, 0, v.size() - 1, summ);
    auto stop1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time1 = stop1 - start1;
    std::cout << "Single thread result: " << summ << "   array size: " << v.size() << "\n";
    std::cout << "Elapsed time = " << elapsed_time1.count() << " sec\n";
    std::cout << "\n\n\n";

    //std::cout << multi_thread_summ(v,0,v.size()-1,threads_num) << "\n";

    for (int i = 1; i < 100; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Threads number: " << i << " result: " << multi_thread_summ(v, 0, v.size() - 1, i) << "\n";
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = stop - start;
        std::cout << "Elapsed time = " << elapsed_time.count() << " sec\n";
    }


    return 0;
}
