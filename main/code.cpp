#include "command.hpp"
int main() {
    init();
    try {
        cmd();
    } catch (string s) {
        cout << s << endl;
    } catch (const char *s) {
        cout << s << endl;
    }
    return 0;
}