#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <limits>
#include <cstdlib>

class Base
{
protected:
    std::string name;

public:
    Base(const std::string &Name)
        : name(Name) {}

    std::string getName() const
    {
        return name;
    }
};

class Student : public Base
{
private:
    std::string SRN;

public:
    Student(const std::string &studentName, const std::string &srn)
        : Base(studentName), SRN(srn) {}

    std::string getStudentSRN() const
    {
        return SRN;
    }
};

class Teacher : public Base
{
private:
    std::string teacherId;

public:
    Teacher(const std::string &teacherName, const std::string &id)
        : Base(teacherName), teacherId(id) {}

    std::string getTeacherID() const
    {
        return teacherId;
    }
};

int getCSVEntryCount(const std::string &csvFileName)
{
    std::ifstream inputFile(csvFileName);
    if (!inputFile.is_open())
    {
        std::cerr << "Unable to open the CSV file." << std::endl;
        return 0;
    }

    int entryCount = 0;
    std::string line;
    while (std::getline(inputFile, line))
    {
        // Ignore empty lines
        if (!line.empty())
        {
            entryCount++;
        }
    }

    inputFile.close();
    return entryCount;
}

class AttendanceManager
{
private:
    std::vector<Student> students;
    std::vector<Teacher> teachers;
    std::string fileName;

public:
    AttendanceManager(const std::string &filename)
        : fileName(filename) {}
    friend void printCSVRows(const AttendanceManager &csvFile);
    void addStudent(const Student &student)
{
    // Check if the student with the same SRN already exists
    for (const Student& existingStudent : students)
    {
        if (existingStudent.getStudentSRN() == student.getStudentSRN())
        {
            std::cout << "Student with the same SRN already exists. Duplicate entry not allowed." << std::endl;
            return;
        }
    }

    // Check if the student with the same name already exists
    for (const Student& existingStudent : students)
    {
        if (existingStudent.getName() == student.getName())
        {
            std::cout << "Student with the same name already exists. Duplicate entry not allowed." << std::endl;
            return;
        }
    }

    students.push_back(student);
    std::ofstream outputFile(fileName, std::ios::app);
    if (outputFile.is_open())
    {
        int entryCount = getCSVEntryCount(fileName);
        outputFile << entryCount << "," << student.getStudentSRN() << "," << student.getName() << "\n";
        outputFile.close();
    }
    else
    {
        std::cerr << "Unable to open the CSV file for writing." << std::endl;
    }
}


    void addTeacher(const Teacher &teacher)
    {
        teachers.push_back(teacher);
    }

    std::string trim(const std::string &str)
    {
        std::string result = str;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch)
                                                  { return !std::isspace(ch); }));
        result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch)
                                  { return !std::isspace(ch); })
                         .base(),
                     result.end());
        return result;
    }

    void markAttendance(const std::string &classId)
    {

        std::cout << "Marking attendance for class " << classId << std::endl;

        std::fstream file(fileName);
        if (!file.is_open())
        {
            std::cerr << "Unable to open the CSV file." << std::endl;
            return;
        }

        std::string headings;
        std::getline(file, headings);

        std::vector<std::string> updatedLines;
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            std::stringstream ss(line);
            std::string column;
            std::vector<std::string> columns;
            while (std::getline(ss, column, ','))
            {
                columns.push_back(column);
            }

            if (columns.size() >= 3)
            {
                std::cout << "Name: " << columns[2] << ", Student ID: " << columns[1] << std::endl;

                std::string input;
                std::cout << "Enter attendance (0 for absent, 1 for present) for student " << columns[1] << ": ";
                std::cin >> input;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (input == "0" || input == "1")
                {
                    columns.push_back(input); // Add the attendance value to the end of the columns
                }
                else
                {
                    std::cout << "Invalid attendance input. Skipping." << std::endl;
                }
            }

            std::string updatedLine;
            for (const std::string &col : columns)
            {
                updatedLine += col + ",";
            }
            updatedLine.pop_back();

            updatedLines.push_back(updatedLine);
        }

        file.close();

        std::ofstream outputFile(fileName);
        if (outputFile.is_open())
        {
            outputFile << headings << "\n";

            for (const std::string &line : updatedLines)
            {
                outputFile << line << "\n";
            }

            outputFile.close();
        }
        else
        {
            std::cerr << "Unable to open the CSV file for writing." << std::endl;
        }
    }
};

void printCSVRows(const AttendanceManager &csvFile)
{
    std::ifstream inputFile(csvFile.fileName);
    if (!inputFile.is_open())
    {
        std::cerr << "Unable to open the CSV file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line))
    {
        std::cout << line << std::endl;
    }

    inputFile.close();
}

int main()
{
    std::string csvFileName;
    const char* envFileName = std::getenv("ATTENDANCE_FILE"); // Retrieve the file name from the environment variable
    if (envFileName != nullptr)
    {
        csvFileName = envFileName;
    }
    else
    {
        std::cerr << "Environment variable 'ATTENDANCE_FILE' not set. Defaulting to 'attendance.csv'." << std::endl;
        csvFileName = "attendance.csv"; // Default file name
    }

    try
    {
        AttendanceManager attendanceManager(csvFileName);
        std::string tname;
        std::cout << "To gain teacher access enter name: " << std::endl;
        std::cin >> tname;
        if (tname == "Anand")
        {

            while (true)
            {
                std::cout << "1. Add student\n2. Mark attendance\n3. Print attendance\n4. Exit\n";
                int choice;
                try {
        std::cin >> choice;

        if (std::cin.fail()) {
            throw std::runtime_error("Invalid input.");
        }
    } catch (std::exception& e) {
        std::cout << e.what() << " Setting choice to 0." << std::endl;
        choice = 0;
        std::cin.clear();  // Clear the error state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Ignore the remaining input
    }

                if (choice == 1)
                {
                    std::string name, id;
                    std::cout << "Enter name and id: ";
                    std::cin >> name >> id;
                    Student student(name, id);
                    attendanceManager.addStudent(student);
                }
                else if (choice == 2)
                {
                    attendanceManager.markAttendance("ClassA");
                }
                else if (choice == 3)
                {
                    std::string csvFileName;
                    std::cout << "Enter name of csv file: ";
                    std::cin >> csvFileName;

                    try
                    {
                        if (csvFileName != "attendance.csv" && csvFileName != "att2.csv")
                        {
                            throw std::runtime_error("Invalid file name entered.");
                        }

                        AttendanceManager csvFile(csvFileName);
                        printCSVRows(csvFile);
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "Exception: " << e.what() << std::endl;
                    }
                }
                else if (choice == 4)
                {
                    break;
                }
            }
        }
        else
        {
            std::cout << "Access denied: Unauthorized access. You are a student" << std::endl;
            printCSVRows(attendanceManager);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }



    return 0;
}