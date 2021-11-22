#include <iostream>
extern "C" float E (int x);
extern "C" float Square (float A, float B);

int main () {
    int command;
    while (printf("Please enter your command: ") && (scanf("%d", &command)) != EOF) {
        switch (command) {
            case 1: {
                int x;
                std:: cin >> x;
                float exp = E(x);
                if (exp == -1) {
                    std:: cout << "Please enter a positive accuracy!" << std:: endl;
                }
                else {
                    std:: cout << "An approximate exponent value is: " << exp << std:: endl;
                }
                break;
            }
            case 2: {
                float A, B, square;
                std:: cin >> A >> B;
                square = Square(A, B);
                if (square == -1) {
                    std:: cout << "Please enter positive variables!" << std:: endl;
                }
                else {
                    std:: cout << "The square of your figure is: " << square << std:: endl;
                }
                break;
            }
            default:
                std:: cout << "You must enter 1 or 2!" << std:: endl;
                break;
        }
    }
    return 0;
}