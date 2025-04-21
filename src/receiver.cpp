#include "message.h"

int main() {
	setlocale(LC_ALL, "ru");
    std::string fileName;
    std::cout << "Введите имя бинарного файла: ";
	std::cin >> fileName;

	std::ofstream file(fileName, std::ios::binary | std::ios::trunc);
	Message empty = { "" };

	file.write(reinterpret_cast<char*>(&empty), sizeof(Message));
	file.close();

	HANDLE fileMutex = CreateMutexA(NULL, FALSE, "MyFileMutex");
	HANDLE canRead = CreateEventA(NULL, FALSE, FALSE, "CanRead");
	HANDLE canWrite = CreateEventA(NULL, FALSE, TRUE, "CanWrite");

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	std::string cmd = "sender.exe " + fileName;
	if (!CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		std::cerr << "Ошибка запуска sender.exe\n";
		return 1;
	}

    std::cout << "Ожидание сообщений от sender...\n";

    while (true) {
        std::string command;
        std::cout << "Введите команду (read/exit): ";
        std::cin >> command;

        if (command == "exit") break;
        else if (command == "read") {
            WaitForSingleObject(canRead, INFINITE);
            WaitForSingleObject(fileMutex, INFINITE);

            std::ifstream fin(fileName, std::ios::binary);
            Message msg;
            fin.read(reinterpret_cast<char*>(&msg), sizeof(Message));
            fin.close();

            ReleaseMutex(fileMutex);
            SetEvent(canWrite); 

            std::cout << "Получено сообщение: " << msg.text << "\n";
        }
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(fileMutex);
    CloseHandle(canRead);
    CloseHandle(canWrite);

    return 0;
}