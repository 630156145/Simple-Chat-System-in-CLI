#include <iostream>
using namespace std;

int s_main();
int c_main();

int main(int argc, char* argv[]) {
    cout << "Enter 'c' to start client, 's' to start server: ";
    string input;
    cin >> input;
    if (input=="s") s_main();
    else if (input=="c") c_main();
    else cout << "input err" << endl;

    return 0;

}