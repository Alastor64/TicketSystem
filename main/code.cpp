#include "command.hpp"
#include "user.hpp"
int main() {
    user_init();
    try {
        cmd();
    } catch (string s) {
        cout << s << endl;
    } catch (const char *s) {
        cout << s << endl;
    }
    return 0;
}