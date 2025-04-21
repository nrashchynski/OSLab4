#include "message.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Неверные аргументы. Использование: sender.exe <fileName>\n";
        return 1;
    }

    std::string fileName = argv[1];

    HANDLE fileMutex = OpenMutexA(SYNCHRONIZE, FALSE, "MyFileMutex");
    HANDLE canRead = OpenEventA(EVENT_MODIFY_STATE, FALSE, "CanRead");
    HANDLE canWrite = OpenEventA(SYNCHRONIZE, FALSE, "CanWrite");

    if (!fileMutex || !canRead || !canWrite) {
        std::cerr << "Не удалось открыть объекты синхронизации\n";
        return 1;
    }

    while (true) {
        std::string command;
        std::cout << "Введите команду (send/exit): ";
        std::cin >> command;

        if (command == "exit") break;
        else if (command == "send") {
            std::cout << "Введите сообщение (макс. 19 символов): ";
            std::string msgInput;
            std::cin.ignore();
            std::getline(std::cin, msgInput);
            if (msgInput.length() >= MAX_MESSAGE_LENGTH) {
                msgInput = msgInput.substr(0, MAX_MESSAGE_LENGTH - 1);
            }

            WaitForSingleObject(canWrite, INFINITE);
            WaitForSingleObject(fileMutex, INFINITE);

            Message msg = {};
            strcpy_s(msg.text, msgInput.c_str());

            std::ofstream fout(fileName, std::ios::binary | std::ios::trunc);
            fout.write(reinterpret_cast<char*>(&msg), sizeof(Message));
            fout.close();

            ReleaseMutex(fileMutex);
            SetEvent(canRead);
        }
    }

    CloseHandle(fileMutex);
    CloseHandle(canRead);
    CloseHandle(canWrite);
    return 0;
}