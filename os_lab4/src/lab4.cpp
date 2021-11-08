#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <string>
#include <sstream>
#include <signal.h>
#include <pthread.h>
#define ll long long
int main() {
    int a;
    std::cout << "Congrats, you are in parent process. Please enter amount of strings: " << std::endl;
    std::cin >> a;
    int less_than_ten = 0;
    int more_than_ten = 0;
    int first_pos = 0;
    int second_pos = 0;
    int first_length = 0;
    int second_length = 0;
    int fd1;
    int fd2;
    std::fstream fs;
    std::string path_child1, path_child2;
    std::cout << "Enter name of file for first child: " << std::endl;
    std::cin >> path_child1;
    std::cout << "For second child: " << std::endl;
    std::cin >> path_child2;
    std::string string;
    if ((fd1 = open("f1.txt", O_RDWR| O_CREAT, 0777)) == -1)
    {
        std::cout << "Error: can not open the f1.txt. Try again later." << std::endl;
        exit(EXIT_FAILURE);
    }
    if ((fd2 = open("f2.txt", O_RDWR| O_CREAT, 0777)) == -1)
    {
        std::cout << "Error: can not open the f2.txt. Try again later." << std::endl;
        exit(EXIT_FAILURE);
    }
    char *mapped_file1 = (char *)mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0); // при помощи мемори маппа отображаем mapped file на оперативную память
    char *mapped_file2 = (char *)mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0); // при помощи мемори маппа отображаем mapped file на оперативную память
    if (mapped_file1 == MAP_FAILED)
    {
        std::cout << "An error with mmap function one has been detected" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (mapped_file2 == MAP_FAILED)
    {
        std::cout << "An error with mmap function two has been detected" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Good. Please enter your strings: " << std::endl;
    while (a > 0)
    {
        std::cin >> string;
        string = string + "\n";
        if (string.size() <= 10)
        {
            less_than_ten++;
            first_length += string.size();
            if (ftruncate(fd1, first_length))
            {
                std::cout << "Error during ftrancate with mf1 has been detected" << std::endl;
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < string.size(); ++i)
            {
                mapped_file1[first_pos++] = string[i];
            }
        }
        else
        {
            more_than_ten++;
            second_length += string.size();
            if (ftruncate(fd2, second_length))
            {
                std::cout << "Error during ftrancate with mf2 has been detected" << std::endl;
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < string.size(); ++i)
            {
                mapped_file2[second_pos++] = string[i];
            }
        }
        a--;
    }
    int first_identificator = fork();
    if (first_identificator == -1)
    {
        std::cout << "Fork error!" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (first_identificator == 0)
    {
        fs.open(path_child1, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!fs.is_open())
        {
            exit(EXIT_FAILURE);
        }
        std::cout << "Congrats, you are in child #1 process" << std::endl;
        int i = 0;
        while (less_than_ten > 0)
        {
            std::string string;
            while (mapped_file1[i] != '\n')
            {
                string += mapped_file1[i];
                i++;
            }
            if (mapped_file1[i] == '\n')
                i++;
            int x = 0;
            while (x < string.size())
            {
                while ((string[x] == char(65)) || (string[x] == char(69)) || (string[x] == char(73)) || (string[x] == char(79)) ||
                       (string[x] == char(85)) || (string[x] == char(89)) || (string[x] == char(97)) || (string[x] == char(101)) ||
                       (string[x] == char(105)) || (string[x] == char(111)) || (string[x] == char(117)) || (string[x] == char(121)))
                {
                    string.erase(string.begin() + x);
                }
                x++;
            }
            fs << string << std::endl;
            less_than_ten--;
        }
    }
    else
    {
        int second_identificator = fork();
        if (second_identificator == -1)
        {
            std::cout << "Fork error!" << std::endl;
            return 4;
        }
        else if (second_identificator == 0)
        {
            fs.open(path_child2, std::fstream::in | std::fstream::out | std::fstream::app);
            if (!fs.is_open())
            {
                exit(EXIT_FAILURE);
            }
            std::cout << "Congrats, you are in child #2 process" << std::endl;
            int i = 0;
            while (more_than_ten > 0)
            {
                std::string string;
                while (mapped_file2[i] != '\n')
                {
                    string += mapped_file2[i];
                    i++;
                }
                if (mapped_file2[i] == '\n')
                    i++;
                int x = 0;
                while (x < string.size())
                {
                    while ((string[x] == char(65)) || (string[x] == char(69)) || (string[x] == char(73)) || (string[x] == char(79)) ||
                           (string[x] == char(85)) || (string[x] == char(89)) || (string[x] == char(97)) || (string[x] == char(101)) ||
                           (string[x] == char(105)) || (string[x] == char(111)) || (string[x] == char(117)) || (string[x] == char(121)))
                    {
                        string.erase(string.begin() + x);
                    }
                    x++;
                }
                fs << string << std::endl;
                more_than_ten--;
            }
        }
        else
        {
            if (munmap(mapped_file1, getpagesize()) == -1)
            {
                std::cout << "Munmap1 error has been dected!" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (munmap(mapped_file2, getpagesize()) == -1)
            {
                std::cout << "Munmap2 error has been dected!" << std::endl;
                exit(EXIT_FAILURE);
            }
            close(fd1);
            close(fd2);
            remove("f1.txt");
            remove("f2.txt");
            return 0;
        }
    }
}