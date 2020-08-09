#include <iostream>
#include <cstdlib>
using namespace std;

int s_main();
int c_main();

int main(int argc, char* argv[]) {
    cout << "Enter 'c' to start client, 's' to start server: ";
    string input;
    cin >> input;
    while (input!="s" && input!="c") {
        cout << "You can enter only 'c' and 's', try again: ";
        cin >> input;
    }
    if (input=="s") s_main();
    else if (input=="c") c_main();
    else cout << "error" << endl;

    system("pause");

    return 0;
}