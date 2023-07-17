#include <iostream>
#include <Windows.h>
#include <stdio.h>

using namespace std;

// Нити.
HANDLE threads[2]; 
// Событие.
HANDLE MyEvent; 
// Дескрипторы для записи данных в канал и чтения данных из канала.
HANDLE pipw = NULL, pipr = NULL;

// Первая нить.
DWORD WINAPI Thread_1(LPVOID lpParam) {
	int param = *((int*)lpParam);
	cout << "Thread " << param << " started.\n";
	DWORD waitResult = WaitForSingleObject(MyEvent, INFINITE);
	if (waitResult == WAIT_OBJECT_0) {
		cout << "Thread 1 event was signaled.\n";
	}
	else {
		cout << "Thread 1 event wait failed with error code: "<< GetLastError() << '\n';
	}

	DWORD dwRead;
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	if (!ReadFile(pipr, buffer, 256, &dwRead, NULL)) {
		cout << "Thread 1 failed to read from pipe. Error code: " << GetLastError() << endl;
		return 5;
	}
	else {
		cout << "Thread 1 read from pipe.\n";
	}
	cout << "Thread 1 received the message: " << buffer << '\n';
	cout << "Thread " << param << " completed.\n";
	return 0;
}
// Вторая нить.
DWORD WINAPI Thread_2(LPVOID lpParam) {
	int param = *((int*)lpParam);
	cout << "Thread " << param << " started.\n";
	if (!SetEvent(MyEvent)) {
		cout << "SetEvent failed " << GetLastError() << '\n';
	}
	else {
		cout << "SetEvent completed successfully\n";
	}
	const char* message = "Hello  thread 1!";

	DWORD dwWritten;
	if (!WriteFile(pipw, message, strlen(message), &dwWritten, NULL)) {
		cout << "Thread 2 failed to write to pipe. Error code: " << GetLastError() << '\n';
		return 6;
	}
	else {
		cout << "Thread 2 wrote to pipe\n";
	}

	cout << "Thread " << param << " completed.\n";
	return 0;
}


int main()
{
	// Данные нитей.
	int Data_Of_Tread_1 = 1;
	int Data_Of_Tread_2 = 2;
	// Создаем событие.
	MyEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("MyEvent"));
	if (MyEvent == NULL) {
		cout << "Failed to create event\n";
		return 1;
	}
	// Создаем анонимный канал.
	if (!CreatePipe(&pipr, &pipw, NULL, 0)) {
		cout << "Failed to create pipe\n";
		return 2;
	}
	// Создаем первую нить.
	if (!(threads[0] = CreateThread(NULL, 0, Thread_1, &Data_Of_Tread_1, 0, NULL))) {
		cout << "Failed to create first thread\n";
		return 3;
	}
	// Создаем вторую нить.
	if (!(threads[1] = CreateThread(NULL, 0, Thread_2, &Data_Of_Tread_2, 0, NULL))) {
		cout << "Failed to create second thread\n";
		return 4;
	}
	// Ждем заверешения работы нитей.
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);
	// Осовбождаем ресурсы.
	CloseHandle(threads[0]);
	CloseHandle(threads[1]);
	CloseHandle(pipr);
	CloseHandle(pipw);
	CloseHandle(MyEvent);
	return 0;
}
