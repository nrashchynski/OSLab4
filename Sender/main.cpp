#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Message.h"

using namespace std;

const wchar_t* MUTEX_NAME = L"GlobalMsgMutex";

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: Sender <filename> <id> <messageCount>" << endl;
        return 1;
    }

    string filename = argv[1];
    int senderId = atoi(argv[2]);
    int messageCount = atoi(argv[3]);

    wstringstream evName;
    evName << L"EventReady_" << senderId;
    HANDLE readyEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, evName.str().c_str());
    if (!readyEvent) {
        cerr << "[Sender " << senderId << "] Failed to open ready event." << endl;
        return 1;
    }

    HANDLE mutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME);
    if (!mutex) {
        cerr << "[Sender " << senderId << "] Failed to open mutex." << endl;
        return 1;
    }

    SetEvent(readyEvent);

    string cmd;
    while (true) {
        cout << "[Sender " << senderId << "] Enter command (send/exit): ";
        cin >> cmd;

        if (cmd == "send") {
            cout << "Enter message: ";
            cin.ignore();
            string msgText;
            getline(cin, msgText);
            if (msgText.size() >= MAX_MSG_LEN) msgText = msgText.substr(0, MAX_MSG_LEN - 1);

            WaitForSingleObject(mutex, INFINITE);

            fstream file(filename, ios::in | ios::out | ios::binary);
            Message msg;
            bool written = false;

            for (int pos = 0; pos < messageCount; ++pos) {
                file.seekg(pos * sizeof(Message));
                file.read((char*)&msg, sizeof(Message));
                if (msg.isEmpty) {
                    file.seekp(pos * sizeof(Message));
                    strcpy_s(msg.text, msgText.c_str());
                    msg.isEmpty = false;
                    file.write((char*)&msg, sizeof(Message));
                    written = true;
                    break;
                }
            }

            file.close();
            ReleaseMutex(mutex);

            if (!written) {
                cout << "[Sender " << senderId << "] No free slot. Message not sent." << endl;
            }
        }
        else if (cmd == "exit") {
            break;
        }
    }

    CloseHandle(mutex);
    CloseHandle(readyEvent);
    return 0;
}
