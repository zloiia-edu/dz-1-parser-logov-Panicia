/**
 * @file main.cpp
 * @author Dmitriy Fedin (fedin.da@gmail.com)
 * @brief Учебная программа для разбора логов
 * @version 0.1
 * @date 2018-10-30
 *
 * @copyright zloiia (c) 2018
 *
 */
#include "pch.h"
#include <iostream> //подключение библиотеки ввода-вывода в терминал
#include <string> //подключение библиотеки для работы со строками
#include <fstream> //подключение библиотеки для работы с файлами как с потоком



using namespace std; //используем пространство имен std, чтобы меньше писать

//коды ошибок
// 0 - нет ошибок
// 1 - ошибка открытия потока
// 2 - кончились данные

/**
 * @brief Интерфейс класса ввода-вывода
 *
 */
class InOut
{
protected:
	int _LastError; //значение кода последней ошибки
public:
	/**
	 * @brief Конструктор по умолчанию для интерфейса.
	 * Необходим, чтобы проинициализировать стартовым значением код ошибки при создании экземпляра
	 */
	InOut() {
		_LastError = 0;
	}

	/**
	 * @brief Метод для чтения кода ошибки из интерфейса
	 * Объявлен как virtual и приравнен к 0, чтобы сделать класс абстрактным
	 *
	 * @return int код ошибки
	 */
	virtual int Read() = 0;

	/**
	 * @brief Метод записи строки в поток
	 * Объявлен как virtual и приравнен к 0, чтобы сделать класс абстрактным
	 *
	 * @param txt строка, которая должна быть отображена в выбранном интерфейсе
	 */
	virtual void Write(string txt) = 0;

	/**
	 * @brief Метод получения кода последней ошибки
	 *
	 * @return int
	 */
	int LastError() const {
		return _LastError;
	}

	/**
	 * @brief Метод открытия интерфейса
	 *
	 */
	virtual void Open() = 0;

	/**
	 * @brief Метод закрытия интерфейса
	 *
	 */
	virtual void Close() = 0;
};

/**
 * @brief Интерфейс обработчика кодов ошибок
 *
 */
class IProcess {
protected:
	InOut * _in; //указатель на входной поток ввода-вывода
	InOut *_out; //указатель на выводной поток ввода-вывода

public:
	/**
	 * @brief Конструктор по умолчанию для интерфейса
	 *
	 */
	IProcess() {
		_in = NULL;
		_out = NULL;
	}

	/**
	 * @brief Деструктор
	 * Объявлен как virtual для того, чтобы дочерние классы более гарантированно могли
	 * его вызвать
	 *
	 */
	virtual ~IProcess() {
		if (_in != NULL)
			delete _in;
		if (_out != NULL)
			delete _out;
	}

	/**
	 * @brief Устанавливаем поток для ввода данных
	 *
	 * @param in указатель на поток ввода данных
	 */
	void SetInput(InOut* in) {
		_in = in;
	}

	/**
	 * @brief Устанавливаем поток вывода данных
	 *
	 * @param out указатель на поток вывода данных
	 */
	void SetOutput(InOut* out) {
		_out = out;
	}

	/**
	 * @brief Метод для вызова процесса обработки данных
	 *
	 */
	virtual void Work() = 0;
};

/**
 * @brief Класс для реализации потока ввода-вывода в терминал
 * Наследуется от класса InOut
 *
 */
class TermIO : public InOut {
public:
	/**
	 * @brief Конструктор по умолчанию. Вызывает конструктор интерфейса для инициализации
	 * всех предыдущих значений
	 *
	 */
	TermIO() : InOut() {
	}

	/**
	 * @brief Реализация метода открытия потока.
	 * Реализация пустая, потому что нет необходимости открывать терминал. Он уже открыт
	 *
	 */
	void Open() override {
	}

	/**
	 * @brief Реализация метода закрытия потока
	 * Реализация пустая, потому что нет необходимости закрывать терминал
	 *
	 */
	void Close() override {
	}

	/**
	 * @brief Реализация чтения кода ошибки из потока
	 *
	 * @return int
	 */
	int Read() override {
		int a = 0; //объявляем временную переменную
		cin >> a; //читаем из потока целое число во временную переменную
		if (a == 0000) {
			exit(0);
		}
		return a; //возвращаем результат
	}

	/**
	 * @brief Реализация записи строки в поток
	 *
	 * @param txt строка для записи
	 */
	void Write(string txt) override {
		cout << "NEW ERROR >>  " << txt << endl; //пишем в поток и переводим каретку
	}
};


/**
 * @brief Класс для реализации потока ввода-вывода в файл
 * Наследуется от класса InOut
 *
 */
class FileIO : public InOut {
private:
	string _filename; //имя файла, с которым мы будем работать
	fstream _fdesc; // дескриптор файла
public:
	/**
	 * @brief Конструктор по умолчанию. Вызывает конструктор интерфейса для инициализации
	 * всех предыдущих значений
	 *
	 */
	FileIO() :InOut() {}

	/**
	 * @brief Конструктор, при помощи которого мы можем передать сразу имя файла
	 *
	 * @param filename
	 */
	FileIO(string filename) :InOut() {
		_filename = filename;
	}

	/**
	 * @brief Реализация чтения кода ошибки из потока
	 *
	 * @return int
	 */

	int Read() override {
		if (!_fdesc.is_open()) { //если файл не открыт, то нам и читать нечего
			_LastError = 1; //ставим ошибку
			return 0; //выходим
		}
		int a = 0;
		_fdesc >> a;
		if (a == 0000) {
			exit(0);
		}
		return a;
	}

	/**
	 * @brief Метод для установки имени файла, с которым мы будем работать.
	 * Дополнительно, открывает файл
	 *
	 * @param filename им файла
	 */
	void SetFilename(string filename) {

		_filename = filename; //сохраняем имя файла
		if (_fdesc.is_open()) { //если файл уже был открыт
			Close();  //закрываем
		}
		Open(); //открываем заново файл
	}

	/**
	 * @brief Реализация метода открытия потока
	 *
	 */
	void Open() override {
		_fdesc.open(_filename, fstream::in | fstream::out); //открываем файл на чтение и запись
	}

	/**
	 * @brief Реализация метода закрытия потока
	 *
	 */
	void Close() override {
		_fdesc.close();
	}


	/**
	 * @brief Реализация метода записи в поток
	 *
	 * @param txt строка для записи
	 */
	void Write(string txt) override {
		if (!_fdesc.is_open()) { //если файл не был открыт, то писать некуда
			_LastError = 1; //ставим ошибку
			return; //выходим
		}
		_fdesc << txt << endl; //пишем в файл и переводим каретку
	}
};

/**
 * @brief Пример реализации обработчика кодов ошибок
 *
 */
class CameraProcess : public IProcess {
public:
	CameraProcess() :IProcess() {}
	void Work() override {
		if (_in == NULL || _out == NULL) {
			cout << "ERROR: No input or output" << endl;
			return;
		}
		while (_in->LastError() != 2) {
			int err_code = _in->Read();
			string errorMessage = "";
			ifstream code("Error codes.txt");
			string cName = "", cNumber = "", A = "";
			if (code) {
				while (!code.eof()) {
					getline(code, A);

					for (int i = A.find("[") + 1; i < A.find(">"); i++) {
						cNumber.push_back(A[i]);
					}
					for (int i = A.find(">") + 1; i < A.find("<"); i++) {
						cName.push_back(A[i]);
					}
					if (err_code == stoi(cNumber)) {
						errorMessage = cName;
					}
					cName = "";
					cNumber = "";
				}
			}
			else cout << "File dose not exist" << endl;
			if (errorMessage == "" ) {
				errorMessage = "UNKNOWN ERROR";
			}
			/*Реализовано чтение из файла кодов ошибок
			при этом файл с ошибками (Error codes) будет выглядеть так:
			[10>WIFI: Can not load<
                        [11>WIFI: Can not find<
                        [20>Bluetooth: Can not load<
                        [21>Bluetooth: Can not find<*/
			_out->Write(errorMessage);
		}
	}
};

/**
 * @brief Точка входа в программу
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char** argv)
{
	///Пример использования получившихся классов
	bool Rock = 0;
	while (Rock == 0) {
		cout << "Choose tipe of reading and writing" << endl;
		cout << "Terminal to File - write <<termtofile>>" << endl;
		cout << "Terminal to Terminal - write <<termtoterm>>" << endl;
		cout << "File to File - write <<filetofile>>" << endl;
		cout << "File to Terminal - write <<filetoterm>>" << endl;
		string voidOFuser = "";
		cin >> voidOFuser;
		if (voidOFuser == "termtofile") {

			InOut* inStream = new TermIO(); //данные берем из терминала
			InOut* outStream = new FileIO("Text.txt"); //кладем в файл 

			inStream->Open(); //открываем потоки
			outStream->Open();

			IProcess* process = new CameraProcess(); //создаем обработчик

			process->SetInput(inStream); //задаем потоки
			process->SetOutput(outStream);

			process->Work(); //работаем

			delete process; //удаляем объект обработчика после работы

			Rock = 1;
		}
		if (voidOFuser == "termtoterm") {
			InOut* inStream = new TermIO();
			InOut* outStream = new TermIO();

			inStream->Open();
			outStream->Open();

			IProcess* process = new CameraProcess(); 

			process->SetInput(inStream); 
			process->SetOutput(outStream);

			process->Work(); 

			delete process;

			Rock = 1;

		}
		if (voidOFuser == "filetofile") {
			InOut* inStream = new FileIO("ErrorFile.txt");
			InOut* outStream = new FileIO("Text.txt");

			inStream->Open();
			outStream->Open();

			IProcess* process = new CameraProcess();

			process->SetInput(inStream);
			process->SetOutput(outStream);

			process->Work();

			delete process;

			Rock = 1;

		}
		if (voidOFuser == "filetoterm") {
			InOut* inStream = new FileIO("ErrorFile.txt");
			InOut* outStream = new TermIO();

			inStream->Open();
			outStream->Open();

			IProcess* process = new CameraProcess();

			process->SetInput(inStream);
			process->SetOutput(outStream);

			process->Work();

			delete process;

			Rock = 1;
		}
		if (voidOFuser != "termtofile" && voidOFuser != "termtoterm" && voidOFuser != "filetofile" && voidOFuser != "filetoterm") {
			cout << "Void does not exist" << endl;
		}
	}

	return 0;
}
