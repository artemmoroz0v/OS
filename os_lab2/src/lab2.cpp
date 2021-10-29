#include <iostream>  
#include <string> 
#include <fstream>
#include "unistd.h"
int main () {
std:: string path_child1, path_child2;
std:: cout << "Congrats, you are in parent process. Please enter name of files for first and second child!" << std:: endl;
std:: cout << "For first child: " << std:: endl;
std:: cin >> path_child1;
std:: cout << "For second child: " << std:: endl;
std:: cin >> path_child2;
std:: fstream fs;
int fd1[2];
int fd2[2];
if (pipe(fd1) == -1) {
    std:: cout << "An error occured with opening the pipe" << std:: endl;
    return 1;
}
if (pipe(fd2) == -1) {
    std:: cout << "An error occured with opening the pipe" << std:: endl;
    return 2;
}
int first_identificator = fork();
if (first_identificator == -1) {
    std:: cout << "Fork error!" << std:: endl;
    return -1; 
}
else if (first_identificator == 0) {
    fs.open(path_child1, std:: fstream:: in | std:: fstream:: out | std:: fstream:: app);
    int a;
    read(fd1[0], &a, sizeof(int));
    std:: cout << "Congrats, you are in child #1 process" << std:: endl;
    while (a > 0) { 
        int size_of_string;
        read(fd1[0], &size_of_string, sizeof(int));
        char string_array[size_of_string]; 
        read(fd1[0], string_array, sizeof(char) * size_of_string);
        std::string string;
        for (int i = 0; i < size_of_string; i++){
            string.push_back(string_array[i]);
        }
        int x = 0;
        while (x < string.size()) {
            while ((string[x] == char(65)) || (string[x] == char(69)) || (string[x] == char(73)) || (string[x] == char(79)) || 
                (string[x] == char(85)) || (string[x] == char(89)) || (string[x] == char(97)) || (string[x] == char(101)) || 
                    (string[x] == char(105)) || (string[x] == char(111)) || (string[x] == char(117)) || (string[x] == char(121))) 
            {
                string.erase(string.begin() + x);
            }
                x++;
            }
        fs << string << std:: endl;
        std::cout << "From child #1, your string is: " << string << std:: endl;
            --a;
    }
        close(fd1[0]);
        close(fd1[1]);
    }
else {
    int second_identificator = fork();
    if (second_identificator == -1) {
        std:: cout << "Fork error!" << std:: endl;
        return -1; 
    }
    else if (second_identificator == 0) {
    fs.open(path_child2, std:: fstream:: in | std:: fstream:: out | std:: fstream:: app);
    int a;
    read(fd2[0], &a, sizeof(int));
    std:: cout << "Congrats, you are in child #2 process!" << std:: endl;
    while (a > 0) {
    int size_of_string;
    read(fd2[0], &size_of_string, sizeof(int));
    char string_array[size_of_string]; 
    read(fd2[0], string_array, sizeof(char) * size_of_string);
    std::string string;
    for (int i = 0; i < size_of_string; i++){
    string.push_back(string_array[i]);
    }
    int x = 0;
    while (x < string.size()) {
        while ((string[x] == char(65)) || (string[x] == char(69)) || (string[x] == char(73)) || (string[x] == char(79)) || 
                (string[x] == char(85)) || (string[x] == char(89)) || (string[x] == char(97)) || (string[x] == char(101)) || 
                    (string[x] == char(105)) || (string[x] == char(111)) || (string[x] == char(117)) || (string[x] == char(121))) 
            {
                string.erase(string.begin() + x);
            }
                x++;
            }
    fs << string << std:: endl;
    std::cout << "From child #2, your string is: " << string << std:: endl;
    a = a - 1;
    }
    close(fd2[0]);
    close(fd2[1]);
    }
else {
    int a;
    std:: cout << "From parent: enter amount of strings" << std::endl;
    std:: cin >> a;
    write(fd1[1], &a, sizeof(int));
    write(fd2[1], &a, sizeof(int));
    std:: cout <<"Good, let's delete vowels in your strings " << a << " times: " << std::endl;
    for (int i = 0; i < a; i++){
    std:: string string;
    std:: cin >> string ;
    int n = string.size();
    char string_array[n];
    for (int i = 0; i < n; i++){
    string_array[i] = string[i];
    }
    if (string.size() <= 10) {
    write(fd1[1], &n, sizeof(int));
    write(fd1[1], string_array, sizeof(char) * n);
    }
    else {
    write(fd2[1], &n, sizeof(int));
    write(fd2[1], string_array, sizeof(char) * n);
    }
    }
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    }
    }
    return 0;
}
