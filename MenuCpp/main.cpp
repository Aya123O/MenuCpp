#include <iostream>
#include <stdexcept>
#include <cstdio>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

using namespace std;

int getch(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[H\033[J";
#endif
}

#define RESET_COLOR "\033[0m"
#define GREEN_COLOR "\033[32m"
#define RED_COLOR "\033[31m"
#define BLUE_COLOR "\033[34m"

struct Employee {
    int id;
    string name;
};

class Stack {
private:
    int size;
    int top;
    Employee* items;

public:
    Stack(int size) {
        this->size = size;
        top = -1;
        items = new Employee[size];
    }

    bool push(const Employee& data) {
        if (top == size - 1) {
            cout << RED_COLOR << "Stack is full! Cannot push more items." << RESET_COLOR << endl;
            return false;
        }
        items[++top] = data;
        return true;
    }

    Employee pop() {
        if (top == -1) {
            throw runtime_error("Stack is empty! Cannot pop.");
        }
        return items[top--];
    }

    void display() const {
        if (top == -1) {
            cout << RED_COLOR << "Stack is empty!" << RESET_COLOR << endl;
            return;
        }
        cout << BLUE_COLOR << "Stack contents:" << RESET_COLOR << endl;
        for (int i = top; i >= 0; i--) {
            cout << "ID: " << items[i].id << ", Name: " << items[i].name << endl;
        }
    }

    bool isIdUnique(int id) const {
        for (int i = 0; i <= top; i++) {
            if (items[i].id == id) {
                return false;
            }
        }
        return true;
    }

    int getSize() const {
        return top + 1;
    }

    ~Stack() {
        delete[] items;
    }
};

void displayMenu(int currentPosition) {
    clearScreen();
    cout << "*----------------------*" << endl;
    if (currentPosition == 0) {
        cout << "* " << GREEN_COLOR << "-> New" << RESET_COLOR << "           *" << endl;
    } else {
        cout << "* " << BLUE_COLOR << "New" << RESET_COLOR << "              *" << endl;
    }
    cout << "*----------------------*" << endl;

    cout << "*----------------------*" << endl;
    if (currentPosition == 1) {
        cout << "* " << GREEN_COLOR << "-> Display" << RESET_COLOR << "       *" << endl;
    } else {
        cout << "* " << BLUE_COLOR << "Display" << RESET_COLOR << "          *" << endl;
    }
    cout << "*----------------------*" << endl;

    cout << "*----------------------*" << endl;
    if (currentPosition == 2) {
        cout << "* " << GREEN_COLOR << "-> Delete" << RESET_COLOR << "        *" << endl;
    } else {
        cout << "* " << BLUE_COLOR << "Delete" << RESET_COLOR << "          *" << endl;
    }
    cout << "*----------------------*" << endl;
}

bool isValidEmployeeName(const string& name) {
    for (char c : name) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

void addEmployee(Stack& stack, int stackSize) {
    if (stack.getSize() == stackSize) {
        cout << RED_COLOR << "Stack is full! Cannot push more items." << RESET_COLOR << endl;
        return;
    }

    Employee emp;

    while (true) {
        cout << "Enter employee ID: ";
        cin >> emp.id;
        if (emp.id < 0) {
            cout << RED_COLOR << "ID cannot be negative!" << RESET_COLOR << endl;
            continue;
        }

        cin.ignore();
        if (!stack.isIdUnique(emp.id)) {
            cout << RED_COLOR << "This ID already exists! Please enter a unique ID." << RESET_COLOR << endl;
        } else {
            break;
        }
    }

    while (true) {
        cout << "Enter employee name: ";
        getline(cin, emp.name);
        if (!isValidEmployeeName(emp.name)) {
            cout << RED_COLOR << "Name must contain only alphabetic characters and spaces!" << RESET_COLOR << endl;
        } else {
            break;
        }
    }

    if (stack.push(emp)) {
        cout << GREEN_COLOR << "Employee added successfully!" << RESET_COLOR << endl;
        cout << "Press Enter to return to the menu...";
        while (getch() != 10);
    } else {
        cout << RED_COLOR << "Failed to add employee!" << RESET_COLOR << endl;
    }
}

int main() {
    int currentPosition = 0, ch, stackSize;

    cout << "Enter the maximum size of the stack: ";
    cin >> stackSize;
    if (stackSize <= 0) {
        cout << RED_COLOR << "Stack size must be greater than 0!" << RESET_COLOR << endl;
        return 0;
    }
    cin.ignore();

    Stack stack(stackSize);
    displayMenu(currentPosition);

    while (true) {
        ch = getch();

        if (ch == 27) {
            ch = getch();
            if (ch == 91) {
                ch = getch();
                if (ch == 65) {
                    currentPosition = (currentPosition - 1 + 3) % 3;
                } else if (ch == 66) {
                    currentPosition = (currentPosition + 1) % 3;
                }
            }
        }
        else if (ch == 10) {
            if (currentPosition == 0) {
                clearScreen();
                addEmployee(stack, stackSize);
                displayMenu(currentPosition);
            }
            else if (currentPosition == 1) {
                clearScreen();
                stack.display();
                cout << "Press Enter to return to the menu...";
                while (getch() != 10);
                displayMenu(currentPosition);
            }
            else if (currentPosition == 2) {
                clearScreen();
                try {
                    Employee emp = stack.pop();
                    cout << GREEN_COLOR << "Popped employee ID: " << emp.id << ", Name: " << emp.name << RESET_COLOR << endl;
                } catch (const runtime_error& e) {
                    cout << RED_COLOR << "Error: " << e.what() << RESET_COLOR << endl;
                }
                cout << "Press Enter to return to the menu...";
                while (getch() != 10);
                displayMenu(currentPosition);
            }
        }

        displayMenu(currentPosition);
    }

    return 0;
}
