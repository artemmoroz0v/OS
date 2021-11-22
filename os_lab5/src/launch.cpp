#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>

int main () {
    void* handle = NULL; //адрес, в будущем нужный нам для получения доступа к библиотеке
    float (*E)(int x); //объявление указателей на функции
    float (*Square)(float A, float B); // объявление указателей на функции
    const char* lib_array[] = {"libd1.so", "libd2.so"};
    int curlib;
    int start_library;
    std:: cout << "Enter start library: " << std:: endl;
    std:: cout << '\t' << "1 for using first library" << std:: endl;
    std:: cout << '\t' << "2 for using second library" << std:: endl;
    std:: cin >> start_library;
    bool flag = true;
    while (flag) {
        if (start_library == 1) {
            curlib = 0;
            flag = false;
        }
        else if (start_library == 2) {
            curlib = 1;
            flag = false;
        }
        else {
            std:: cout << "You should enter only 1 or 2!" << std:: endl;
            std:: cin >> start_library;
        }
    }
    handle = dlopen(lib_array[curlib], RTLD_LAZY); //rtld lazy выполняется поиск только тех символов, на которые есть ссылки из кода
    if (!handle) {
        std:: cout << "An error while opening library has been detected" << std:: endl;
        exit(EXIT_FAILURE);
    }
    E = (float(*)(int))dlsym(handle, "E"); //возвращаем адрес функции из памяти библиотеки
    Square = (float(*)(float, float))dlsym(handle, "Square"); //dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
    int command;
    std:: cout << "Hello there! Please enter your command according to next rules: " << std:: endl;
    std:: cout << '\t' << "0 for changing the contract;" << std:: endl;
    std:: cout << '\t' << "1 for calculating the exponent; " << std:: endl;
    std:: cout << '\t' << "2 for calculating the square; " << std:: endl; 
    while (printf("Please enter your command: ") && (scanf("%d", &command)) != EOF) {
        if (command == 0) {
            dlclose(handle); //освобождает указатель на библиотеку и программа перестает ей пользоваться
            if (curlib == 0) {
                curlib = 1 - curlib;
                handle = dlopen(lib_array[curlib], RTLD_LAZY);
                if (!handle) {
                    std:: cout << "An error while opening library has been detected" << std:: endl;
                    exit(EXIT_FAILURE);
                }
                E = (float(*)(int))dlsym(handle, "E");
                Square = (float(*)(float, float))dlsym(handle, "Square"); 
            }
            else if (curlib == 1) {
                curlib = 1 - curlib;
                handle = dlopen(lib_array[curlib], RTLD_LAZY);
                if (!handle) {
                    std:: cout << "An error while opening library has been detected" << std:: endl;
                    exit(EXIT_FAILURE);
                }
                E = (float(*)(int))dlsym(handle, "E");
                Square = (float(*)(float, float))dlsym(handle, "Square");
            }
        std:: cout << "You have changed contracts!" << std:: endl;
        }
        else if (command == 1) {
            int x;
            std:: cout << "Please enter accuracy x: ";
            std:: cin >> x;
            float exp = E(x);
            if (exp == -1) {
                std:: cout << "Please enter a positive accuracy!" << std:: endl;
            }
            else {
                std:: cout << "An approximate exponent value is: " << exp << std:: endl;
            }
        }
        else if (command == 2) {
            float A, B, square;
            std:: cout << "Please enter sides of your figure: ";
            std:: cin >> A >> B;
            square = Square(A, B);
            if (square == -1) {
                std:: cout << "Please enter positive variables!" << std:: endl;
            }
            else {
                std:: cout << "The square of your figure is: " << square << std:: endl;
            }
        }
        else {
            std:: cout << "You had to enter only 0, 1 or 2!" << std:: endl;
        }    
    }
    dlclose(handle);
    return 0;
}