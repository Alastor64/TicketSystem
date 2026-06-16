#include "command.hpp"
int main() {
    std::ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    init();
    try {
        cmd();
    } catch (string s) {
        cout << s << endl;
    } catch (const char *s) {
        cout << s << endl;
    }
    // cout << "\n\n" << sizeof(order_detailed) << "\n\n";
    // command_cnt();
    // cout << "buySeatNum:" << buySeatNum << endl;
    // cout << "maxPairedTrain" << maxPairedTrain << endl;
    // cout << "maxPassTrain" << maxPassTrain << endl;
    return 0;
}